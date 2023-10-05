#include <image.hxx>
#include <device.hxx>

namespace Engine
{
    namespace tools
    {
        namespace
        {
            imageID image_id{ 0 };
            std::unordered_map<imageID, image *> _images{};
        } // namespace

        image::image( VkExtent3D extend, const VkBufferUsageFlags iUsage, const VkImageTiling tiling, const VkMemoryPropertyFlags mProperties, VkImageAspectFlags aspect, VkFormat format, VkImageCreateInfo ImageCreateInfo ) : id{ ++image_id }
        {
            ImageCreateInfo.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            ImageCreateInfo.imageType     = VK_IMAGE_TYPE_2D;
            ImageCreateInfo.extent        = extend;
            ImageCreateInfo.extent.depth  = ImageCreateInfo.extent.depth ? ImageCreateInfo.extent.depth : 1;
            ImageCreateInfo.mipLevels     = ImageCreateInfo.mipLevels ? ImageCreateInfo.mipLevels : 1;
            ImageCreateInfo.arrayLayers   = ImageCreateInfo.arrayLayers ? ImageCreateInfo.arrayLayers : 1;
            ImageCreateInfo.format        = ImageCreateInfo.format ? ImageCreateInfo.format : VK_FORMAT_R8G8B8A8_SRGB;
            ImageCreateInfo.tiling        = tiling;
            ImageCreateInfo.initialLayout = ImageCreateInfo.initialLayout ? ImageCreateInfo.initialLayout : VK_IMAGE_LAYOUT_UNDEFINED;
            ImageCreateInfo.usage         = iUsage;
            ImageCreateInfo.sharingMode   = ImageCreateInfo.sharingMode ? ImageCreateInfo.sharingMode : VK_SHARING_MODE_EXCLUSIVE;
            ImageCreateInfo.samples       = ImageCreateInfo.samples ? ImageCreateInfo.samples : VK_SAMPLE_COUNT_1_BIT;

            CHECK_RESULT( vkCreateImage( getDevice(), &ImageCreateInfo, ALLOCATION_CALLBACK, &handler ) );

            VkMemoryRequirements MemoryRequirements{};
            vkGetImageMemoryRequirements( getDevice(), handler, &MemoryRequirements );

            VkMemoryAllocateInfo MemoryAllocateInfo{};
            MemoryAllocateInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            MemoryAllocateInfo.allocationSize  = MemoryRequirements.size;
            MemoryAllocateInfo.memoryTypeIndex = memoryTypeIndex( MemoryRequirements.memoryTypeBits, mProperties );

            CHECK_RESULT( vkAllocateMemory( getDevice(), &MemoryAllocateInfo, ALLOCATION_CALLBACK, &memory ) );
            CHECK_RESULT( vkBindImageMemory( getDevice(), handler, memory, 0 ) );

            VkImageViewCreateInfo ImageViewCreateInfo{};
            ImageViewCreateInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            ImageViewCreateInfo.image                           = handler;
            ImageViewCreateInfo.format                          = format;
            ImageViewCreateInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
            ImageViewCreateInfo.components                      = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
            ImageViewCreateInfo.subresourceRange.aspectMask     = aspect;
            ImageViewCreateInfo.subresourceRange.baseMipLevel   = 0;
            ImageViewCreateInfo.subresourceRange.levelCount     = ImageCreateInfo.mipLevels;
            ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
            ImageViewCreateInfo.subresourceRange.layerCount     = ImageCreateInfo.arrayLayers;

            CHECK_RESULT( vkCreateImageView( getDevice(), &ImageViewCreateInfo, ALLOCATION_CALLBACK, &view ) );
            _images[ id ] = this;
        }

        image::image( VkExtent3D extend, VkImage &image, VkImageAspectFlags aspect, VkFormat format, uint32_t mipLevels, uint32_t arrayLayers ) : id( ++image_id )
        {
            handler = image;
            VkImageViewCreateInfo ImageViewCreateInfo{};
            ImageViewCreateInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            ImageViewCreateInfo.image                           = handler;
            ImageViewCreateInfo.format                          = format;
            ImageViewCreateInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
            ImageViewCreateInfo.components                      = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
            ImageViewCreateInfo.subresourceRange.aspectMask     = aspect;
            ImageViewCreateInfo.subresourceRange.baseMipLevel   = 0;
            ImageViewCreateInfo.subresourceRange.levelCount     = mipLevels;
            ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
            ImageViewCreateInfo.subresourceRange.layerCount     = arrayLayers;

            CHECK_RESULT( vkCreateImageView( getDevice(), &ImageViewCreateInfo, ALLOCATION_CALLBACK, &view ) );
            _images[ id ] = this;
        }

        image::image( const uint32_t width, const uint32_t height, const VkBufferUsageFlags iUsage, const VkImageTiling tiling, const VkMemoryPropertyFlags mProperties, VkImageAspectFlags aspect, VkFormat format, VkImageCreateInfo ImageCreateInfo ) : image::image( { width, height }, iUsage, tiling, mProperties, aspect, format, ImageCreateInfo )
        {
        }
        imageID image::getID() const
        {
            return id;
        }

        VkImage image::getImage() const
        {
            return handler;
        }

        VkImageView image::getImageView() const
        {
            return view;
        }

        VkDeviceMemory image::getImageMemory() const
        {
            return memory;
        }

        image::~image()
        {
            vkDestroyImageView( getDevice(), view, ALLOCATION_CALLBACK );
            if( memory == nullptr )
                return;
            vkFreeMemory( getDevice(), memory, ALLOCATION_CALLBACK );
            vkDestroyImage( getDevice(), handler, ALLOCATION_CALLBACK );
            _images[ id ] = nullptr;
        }

        image *getImage( imageID id )
        {
            return _images[ id ];
        }

    } // namespace tools
} // namespace Engine