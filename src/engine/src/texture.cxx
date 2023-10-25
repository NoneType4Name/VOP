#include <texture.hxx>
#define STB_IMAGE_IMPLEMENTATION
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include <stb_image.h>
#include <sampler.hxx>

namespace Engine
{
    namespace tools
    {
        namespace
        {
            textureID _textureID { 0 };
            std::unordered_map<textureID, texture *> _textures;
            struct DescriptorData
            {
                buffer *uniformBuffer { nullptr };
            };
        } // namespace

        texture::texture( const char *path ) :
            id { ++_textureID }
        {
            mips.reserve( 1 );
            mips.push_back( stbi_load( path, &_x, &_y, &_c, STBI_rgb_alpha ) );
            mips.resize( static_cast<uint32_t>( std::floor( std::log2( ( _x > _y ) ? _x : _y ) ) ) + 1 );
            if ( getSampler( mips.size() ) == nullptr )
                new sampler( mips.size() );
            VkImageCreateInfo imgCI {};
            imgCI.mipLevels = mips.size();
            img             = ( new image( _x, _y, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, VK_IMAGE_TILING_OPTIMAL, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_FORMAT_R8G8B8A8_SRGB, imgCI ) )->getID();
            _textures[ id ] = this;
        }

        texture::~texture()
        {
            delete getImage( img );
        }

        void texture::generateMipMap( commandBuffer &cmdBuff )
        {
            VkImageMemoryBarrier ImageMemoryBarrier {};
            ImageMemoryBarrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            ImageMemoryBarrier.image                           = getImage( img )->getImage();
            ImageMemoryBarrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
            ImageMemoryBarrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
            ImageMemoryBarrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
            ImageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
            ImageMemoryBarrier.subresourceRange.layerCount     = 1;
            ImageMemoryBarrier.subresourceRange.levelCount     = 1;
            int32_t w { static_cast<int32_t>( _x ) };
            int32_t h { static_cast<int32_t>( _y ) };

            for ( uint32_t i { 1 }; i < mips.size(); i++ )
            {
                ImageMemoryBarrier.subresourceRange.baseMipLevel = i - 1;
                ImageMemoryBarrier.oldLayout                     = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                ImageMemoryBarrier.newLayout                     = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
                ImageMemoryBarrier.srcAccessMask                 = VK_ACCESS_TRANSFER_WRITE_BIT;
                ImageMemoryBarrier.dstAccessMask                 = VK_ACCESS_TRANSFER_READ_BIT;

                vkCmdPipelineBarrier( cmdBuff.getHandle(), VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, 0, 0, 0, 1, &ImageMemoryBarrier );

                VkImageBlit ImageBlit {};
                ImageBlit.srcOffsets[ 0 ]               = { 0, 0, 0 };
                ImageBlit.srcOffsets[ 1 ]               = { _x, _y, 1 };
                ImageBlit.srcSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
                ImageBlit.srcSubresource.baseArrayLayer = 0;
                ImageBlit.srcSubresource.mipLevel       = i - 1;
                ImageBlit.srcSubresource.layerCount     = 1;
                ImageBlit.dstOffsets[ 0 ]               = { 0, 0, 0 };
                ImageBlit.dstOffsets[ 1 ]               = { w - 1 ? w /= 2 : 1, h - 1 ? h /= 2 : 1, 1 };
                ImageBlit.dstSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
                ImageBlit.dstSubresource.baseArrayLayer = 0;
                ImageBlit.dstSubresource.mipLevel       = i;
                ImageBlit.dstSubresource.layerCount     = 1;
                vkCmdBlitImage( cmdBuff.getHandle(), getImage( img )->getImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, getImage( img )->getImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &ImageBlit, VK_FILTER_LINEAR );

                ImageMemoryBarrier.oldLayout     = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
                ImageMemoryBarrier.newLayout     = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                ImageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                ImageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

                vkCmdPipelineBarrier( cmdBuff.getHandle(), VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, 0, 0, 0, 1, &ImageMemoryBarrier );
            }
            ImageMemoryBarrier.subresourceRange.baseMipLevel = mips.size() - 1;
            ImageMemoryBarrier.oldLayout                     = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            ImageMemoryBarrier.newLayout                     = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            ImageMemoryBarrier.srcAccessMask                 = VK_ACCESS_TRANSFER_WRITE_BIT;
            ImageMemoryBarrier.dstAccessMask                 = VK_ACCESS_SHADER_READ_BIT;

            vkCmdPipelineBarrier( cmdBuff.getHandle(), VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, 0, 0, 0, 1, &ImageMemoryBarrier );
        }

        void init()
        {
        }

        void setupDescriptorSet()
        {
        }

        uint32_t texture::getMIPlevels()
        {
            return mips.size();
        }

        const textureID texture::getID() const
        {
            return id;
        }

        const uint8_t *texture::getHandle( const size_t mip_level ) const
        {
            return mips[ mip_level ];
        }

        const uint32_t texture::getWidth() const
        {
            return static_cast<uint32_t>( _x );
        }

        const uint32_t texture::getHeight() const
        {
            return static_cast<uint32_t>( _y );
        }

        const uint32_t texture::getChanels() const
        {
            return static_cast<uint32_t>( _c );
        }

        texture *getTexture( const textureID id )
        {
            return _textures[ id ];
        }

    } // namespace tools
} // namespace Engine
