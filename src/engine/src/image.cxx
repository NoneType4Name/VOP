#include <image.hxx>
#include <device.hxx>

namespace Engine
{
    image::image( types::device device, VkImageCreateInfo ImageCreateInfo, VkImageViewCreateInfo ImageViewCreateInfo )
    {
        // ImageCreateInfo.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        // ImageCreateInfo.imageType     = VK_IMAGE_TYPE_2D;
        // ImageCreateInfo.extent        = extend;
        // ImageCreateInfo.extent.depth  = ImageCreateInfo.extent.depth ? ImageCreateInfo.extent.depth : 1;
        // ImageCreateInfo.mipLevels     = ImageCreateInfo.mipLevels ? ImageCreateInfo.mipLevels : 1;
        // ImageCreateInfo.arrayLayers   = ImageCreateInfo.arrayLayers ? ImageCreateInfo.arrayLayers : 1;
        // ImageCreateInfo.format        = ImageCreateInfo.format ? ImageCreateInfo.format : VK_FORMAT_R8G8B8A8_SRGB;
        // ImageCreateInfo.tiling        = tiling;
        // ImageCreateInfo.initialLayout = ImageCreateInfo.initialLayout ? ImageCreateInfo.initialLayout : VK_IMAGE_LAYOUT_UNDEFINED;
        // ImageCreateInfo.usage         = iUsage;
        // ImageCreateInfo.sharingMode   = ImageCreateInfo.sharingMode ? ImageCreateInfo.sharingMode : VK_SHARING_MODE_EXCLUSIVE;
        // ImageCreateInfo.samples       = ImageCreateInfo.samples ? ImageCreateInfo.samples : VK_SAMPLE_COUNT_1_BIT;
        ImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        CHECK_RESULT( vkCreateImage( data->device->data->handle, &ImageCreateInfo, ALLOCATION_CALLBACK, &ImageViewCreateInfo.image ) );
        ImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        // memoryIndex = this->device->data->setImageMemory( handle, mProperties );

        // VkImageViewCreateInfo ImageViewCreateInfo {};
        // ImageViewCreateInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        // ImageViewCreateInfo.image                           = data->handle;
        // ImageViewCreateInfo.format                          = format;
        // ImageViewCreateInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
        // ImageViewCreateInfo.components                      = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
        // ImageViewCreateInfo.subresourceRange.aspectMask     = aspect;
        // ImageViewCreateInfo.subresourceRange.baseMipLevel   = 0;
        // ImageViewCreateInfo.subresourceRange.levelCount     = ImageCreateInfo.mipLevels;
        // ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        // ImageViewCreateInfo.subresourceRange.layerCount     = ImageCreateInfo.arrayLayers;
        CHECK_RESULT( vkCreateImageView( data->device->data->handle, &ImageViewCreateInfo, ALLOCATION_CALLBACK, &data->view ) );
        DEFINE_DATA_FIELD( device, ImageCreateInfo, ImageViewCreateInfo );
        data->device->data->images.insert( this );
    }

    image::image( types::device device, types::image image, VkImageViewCreateInfo ImageViewCreateInfo )
    {
        DEFINE_DATA_FIELD( device, image, ImageViewCreateInfo );
        data->device->data->images.insert( this );
        ImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        CHECK_RESULT( vkCreateImageView( data->device->data->handle, &ImageViewCreateInfo, ALLOCATION_CALLBACK, &data->view ) );
    }

    image::~image()
    {
        for ( auto &view : data->views )
            delete view;
        vkDestroyImageView( data->device->data->handle, data->view, ALLOCATION_CALLBACK );
        if ( data->ImageInfo.sType )
            vkDestroyImage( data->device->data->handle, data->ImageViewInfo.image, ALLOCATION_CALLBACK );
        data->device->data->images.erase( this );
    }

    void image::transition( VkImageLayout newLayout, VkDependencyFlags dependencyFlags, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, uint32_t srcQueueFamilyIndex, uint32_t dstQueueFamilyIndex )
    {
        VkImageMemoryBarrier ImageMemoryBarrier { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
        ImageMemoryBarrier.image               = data->ImageViewInfo.image;
        ImageMemoryBarrier.oldLayout           = data->ImageInfo.initialLayout;
        ImageMemoryBarrier.newLayout           = newLayout;
        ImageMemoryBarrier.srcQueueFamilyIndex = srcQueueFamilyIndex;
        ImageMemoryBarrier.dstQueueFamilyIndex = dstQueueFamilyIndex;
        if ( newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL )
        {
            ImageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
            if ( data->ImageInfo.format == VK_FORMAT_D32_SFLOAT_S8_UINT || data->ImageInfo.format == VK_FORMAT_D24_UNORM_S8_UINT )
                ImageMemoryBarrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }
        else
            ImageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

        if ( data->ImageInfo.initialLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL )
        {
            ImageMemoryBarrier.srcAccessMask = 0;
            ImageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            if ( !srcStageMask )
                srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            if ( !dstStageMask )
                dstStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if ( data->ImageInfo.initialLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL )
        {
            ImageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            ImageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            if ( !srcStageMask )
                srcStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
            if ( !dstStageMask )
                dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else if ( data->ImageInfo.initialLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL )
        {
            ImageMemoryBarrier.srcAccessMask = 0;
            ImageMemoryBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            if ( !srcStageMask )
                srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            if ( !dstStageMask )
                dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        }
        else
        {
            SPDLOG_CRITICAL( std::format( "Unsupported layout transition, old layout: {}, new layout: {}", std::to_string( data->ImageInfo.initialLayout ), std::to_string( newLayout ) ) );
            return;
        }
        ImageMemoryBarrier.subresourceRange.layerCount     = data->ImageInfo.arrayLayers;
        ImageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
        ImageMemoryBarrier.subresourceRange.levelCount     = data->ImageInfo.mipLevels;
        ImageMemoryBarrier.subresourceRange.baseMipLevel   = 0;

        commandBuffer command { data->device, data->device->data->grapchicPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, data->device->data->queuesSet.graphic };
        command.begin();
        vkCmdPipelineBarrier( command.handle, srcStageMask, dstStageMask, VK_DEPENDENCY_BY_REGION_BIT, 0, 0, 0, 0, 1, &ImageMemoryBarrier );
        command.submit();
    }

    void image::write( std::vector<void *> img, VkExtent3D srcExtend, VkOffset3D srcOffset, VkImageAspectFlags aspectMask, uint32_t mipLevel, uint32_t arrayLayersCount, uint32_t baseArrayLayer, VkMemoryMapFlags flags )
    {
        VkBuffer TransferBuffer;
        std::vector<void *> mapped( img.size() );
        VkBufferCreateInfo bCI { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
        bCI.size                  = img.size();
        bCI.queueFamilyIndexCount = 1;
        bCI.pQueueFamilyIndices   = &data->device->data->queuesSet.transfer.familyIndex.value();
        bCI.usage                 = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        bCI.sharingMode           = VK_SHARING_MODE_EXCLUSIVE;
        vkCreateBuffer( data->device->data->handle, &bCI, ALLOCATION_CALLBACK, &TransferBuffer );
        auto TransferBufferMemoryAddr { data->device->data->memory.allocate( TransferBuffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT ) };
        vkMapMemory( data->device->data->handle, TransferBufferMemoryAddr.memory, TransferBufferMemoryAddr.offset, mapped.size(), flags, mapped.data() );
        memcpy( mapped.data(), img.data(), img.size() );
        vkUnmapMemory( data->device->data->handle, TransferBufferMemoryAddr.memory );

        transition( VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL );

        VkBufferImageCopy copyInfo {};
        copyInfo.bufferOffset                    = 0;
        copyInfo.bufferImageHeight               = 0;
        copyInfo.bufferRowLength                 = 0;
        copyInfo.imageExtent                     = srcExtend.width ? srcExtend : data->ImageInfo.extent;
        copyInfo.imageOffset                     = srcOffset;
        copyInfo.imageSubresource.aspectMask     = aspectMask;
        copyInfo.imageSubresource.baseArrayLayer = baseArrayLayer;
        copyInfo.imageSubresource.layerCount     = arrayLayersCount;
        copyInfo.imageSubresource.mipLevel       = mipLevel;
        commandBuffer commands { data->device, data->device->data->transferPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, data->device->data->queuesSet.transfer };
        commands.begin();
        vkCmdCopyBufferToImage( commands.handle, TransferBuffer, data->ImageViewInfo.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyInfo );
        commands.submit();
    }

    image::DATA_TYPE::DATA_TYPE( types::image parent, types::device device, types::image image, VkImageViewCreateInfo ImageViewCreateInfo ) :
        parent { parent }, device { device }, image { image }, ImageViewInfo { ImageViewCreateInfo }
    {
        if ( image )
            image->data->views.emplace_back( parent );
    }

    image::DATA_TYPE::DATA_TYPE( types::image parent, types::device device, VkImageCreateInfo ImageCreateInfo, VkImageViewCreateInfo ImageViewCreateInfo ) :
        parent { parent }, device { device }, ImageInfo { ImageCreateInfo }, ImageViewInfo { ImageViewCreateInfo }
    {
    }

    image::DATA_TYPE::~DATA_TYPE()
    {
    }
} // namespace Engine