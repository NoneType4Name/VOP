#include <image.hxx>
#include <device.hxx>

namespace Engine
{
    image::image( types::device device, VkImageCreateInfo ImageCreateInfo, VkImageViewCreateInfo ImageViewCreateInfo, VkMemoryPropertyFlags memoryPropertiesFlag ) :
        properties { ImageCreateInfo.extent, ImageCreateInfo.mipLevels, ImageCreateInfo.arrayLayers, ImageCreateInfo.usage, ImageCreateInfo.samples, ImageCreateInfo.imageType, ImageCreateInfo.format, ImageCreateInfo.initialLayout },
        view { ImageViewCreateInfo.viewType, ImageViewCreateInfo.format, ImageViewCreateInfo.subresourceRange, nullptr },
        handle { nullptr }
    {
        DEFINE_DATA_FIELD( device );
        ImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        if ( !ImageViewCreateInfo.image )
        {
            CHECK_RESULT( vkCreateImage( data->device->handle, &ImageCreateInfo, ALLOCATION_CALLBACK, &ImageViewCreateInfo.image ) );
            addres = data->device->memory.allocate( handle, memoryPropertiesFlag );
        }
        else if ( ImageViewCreateInfo.image && ImageCreateInfo.samples != VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM )
        {
            handle = ImageViewCreateInfo.image;
        }

        ImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        CHECK_RESULT( vkCreateImageView( data->device->handle, &ImageViewCreateInfo, ALLOCATION_CALLBACK, &view.handle ) );
        data->device->data->images.insert( this );
    }

    image::image( types::device device, types::image image, VkImageViewCreateInfo ImageViewCreateInfo ) :
        properties { image->properties },
        view { ImageViewCreateInfo.viewType, ImageViewCreateInfo.format, ImageViewCreateInfo.subresourceRange, nullptr },
        handle { image->handle }
    {
        DEFINE_DATA_FIELD( device, image );
        data->device->data->images.insert( this );
        ImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        CHECK_RESULT( vkCreateImageView( data->device->handle, &ImageViewCreateInfo, ALLOCATION_CALLBACK, &view.handle ) );
    }

    image::~image()
    {
        auto v { data->views.begin() };
        while ( v != data->views.end() )
            delete *v++;
        vkDestroyImageView( data->device->handle, view.handle, ALLOCATION_CALLBACK );
        if ( handle )
            vkDestroyImage( data->device->handle, handle, ALLOCATION_CALLBACK );
        data->device->data->images.erase( this );
    }

    void image::transition( types::commandBuffer commandBuffer, VkImageLayout newLayout, VkDependencyFlags dependencyFlags, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, uint32_t srcQueueFamilyIndex, uint32_t dstQueueFamilyIndex )
    {
        VkImageMemoryBarrier ImageMemoryBarrier { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
        ImageMemoryBarrier.image               = handle;
        ImageMemoryBarrier.oldLayout           = properties.layout;
        ImageMemoryBarrier.newLayout           = newLayout;
        ImageMemoryBarrier.srcQueueFamilyIndex = srcQueueFamilyIndex;
        ImageMemoryBarrier.dstQueueFamilyIndex = dstQueueFamilyIndex;
        if ( newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL )
        {
            ImageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
            if ( properties.format == VK_FORMAT_D32_SFLOAT_S8_UINT || properties.format == VK_FORMAT_D24_UNORM_S8_UINT )
                ImageMemoryBarrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }
        else
            ImageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

        if ( properties.layout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL )
        {
            ImageMemoryBarrier.srcAccessMask = 0;
            ImageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            if ( !srcStageMask )
                srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            if ( !dstStageMask )
                dstStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if ( properties.layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL )
        {
            ImageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            ImageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            if ( !srcStageMask )
                srcStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
            if ( !dstStageMask )
                dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else if ( properties.layout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL )
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
            SPDLOG_CRITICAL( std::format( "Unsupported layout transition, old layout: {}, new layout: {}", std::to_string( properties.layout ), std::to_string( newLayout ) ) );
            return;
        }
        ImageMemoryBarrier.subresourceRange.layerCount     = properties.arrayLayers;
        ImageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
        ImageMemoryBarrier.subresourceRange.levelCount     = properties.mipLevels;
        ImageMemoryBarrier.subresourceRange.baseMipLevel   = 0;
        if ( !commandBuffer->data->used )
            commandBuffer->begin();
        vkCmdPipelineBarrier( commandBuffer->handle, srcStageMask, dstStageMask, VK_DEPENDENCY_BY_REGION_BIT, 0, 0, 0, 0, 1, &ImageMemoryBarrier );
    }

    void image::transition( types::commandPool commandPool, types::queue queue, VkImageLayout newLayout, VkDependencyFlags dependencyFlags, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, uint32_t srcQueueFamilyIndex, uint32_t dstQueueFamilyIndex )
    {
        auto cmdB { new commandBuffer { commandPool, queue, VK_COMMAND_BUFFER_LEVEL_PRIMARY } };
        VkFence fence;
        VkFenceCreateInfo fCI { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
        vkCreateFence( commandPool->data->queue->data->device->handle, &fCI, ALLOCATION_CALLBACK, &fence );
        VkSubmitInfo submit {};
        submit.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit.commandBufferCount = 1;
        submit.pCommandBuffers    = &cmdB->handle;
        CHECK_RESULT( vkQueueSubmit( queue->handle, 1, &submit, fence ) );
        CHECK_RESULT( vkWaitForFences( queue->data->device->handle, 1, &fence, true, -1ui64 ) );
        delete cmdB;
    }

    void image::write( types::commandBuffer commandBuffer, std::vector<void *> img, VkExtent3D srcExtend, VkOffset3D srcOffset, VkImageAspectFlags aspectMask, uint32_t mipLevel, uint32_t arrayLayersCount, uint32_t baseArrayLayer, VkMemoryMapFlags flags )
    {
        VkBuffer TransferBuffer;
        std::vector<void *> mapped( img.size() );
        VkBufferCreateInfo bCI { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
        bCI.size                  = img.size();
        bCI.queueFamilyIndexCount = 1;
        bCI.pQueueFamilyIndices   = &commandBuffer->data->queue->familyIndex;
        bCI.usage                 = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        bCI.sharingMode           = VK_SHARING_MODE_EXCLUSIVE;
        CHECK_RESULT( vkCreateBuffer( data->device->handle, &bCI, ALLOCATION_CALLBACK, &TransferBuffer ) );
        auto TransferBufferMemoryAddr { data->device->memory.allocate( TransferBuffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT ) };
        CHECK_RESULT( vkMapMemory( data->device->handle, TransferBufferMemoryAddr.memory, TransferBufferMemoryAddr.offset, mapped.size(), flags, mapped.data() ) );
        memcpy( mapped.data(), img.data(), img.size() );
        vkUnmapMemory( data->device->handle, TransferBufferMemoryAddr.memory );

        transition( commandBuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL );

        VkBufferImageCopy copyInfo {};
        copyInfo.bufferOffset                    = 0;
        copyInfo.bufferImageHeight               = 0;
        copyInfo.bufferRowLength                 = 0;
        copyInfo.imageExtent                     = srcExtend.width ? srcExtend : properties.extent;
        copyInfo.imageOffset                     = srcOffset;
        copyInfo.imageSubresource.aspectMask     = aspectMask;
        copyInfo.imageSubresource.baseArrayLayer = baseArrayLayer;
        copyInfo.imageSubresource.layerCount     = arrayLayersCount;
        copyInfo.imageSubresource.mipLevel       = mipLevel;
        if ( !commandBuffer->data->used )
            commandBuffer->begin();
        vkCmdCopyBufferToImage( commandBuffer->handle, TransferBuffer, handle, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyInfo );
    }

    void image::write( types::commandPool commandPool, types::queue queue, std::vector<void *> img, VkExtent3D srcExtend, VkOffset3D srcOffset, VkImageAspectFlags aspectMask, uint32_t mipLevel, uint32_t arrayLayersCount, uint32_t baseArrayLayer, VkMemoryMapFlags flags )
    {
        auto cmdB { new commandBuffer { commandPool, queue, VK_COMMAND_BUFFER_LEVEL_PRIMARY } };
        write( cmdB, img, srcExtend, srcOffset, aspectMask, mipLevel, arrayLayersCount, baseArrayLayer, flags );
        VkFence fence;
        VkFenceCreateInfo fCI { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
        vkCreateFence( commandPool->data->queue->data->device->handle, &fCI, ALLOCATION_CALLBACK, &fence );
        VkSubmitInfo submit {};
        submit.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit.commandBufferCount = 1;
        submit.pCommandBuffers    = &cmdB->handle;
        CHECK_RESULT( vkQueueSubmit( queue->handle, 1, &submit, fence ) );
        CHECK_RESULT( vkWaitForFences( queue->data->device->handle, 1, &fence, true, -1ui64 ) );
        delete cmdB;
    }

    image::DATA_TYPE::DATA_TYPE( types::image parent, types::device device, types::image image ) :
        parent { parent }, device { device }, parentImage { image }
    {
        if ( image )
            image->data->views.emplace_back( parent );
    }

    image::DATA_TYPE::DATA_TYPE( types::image parent, types::device device ) :
        parent { parent }, device { device }
    {
    }

    image::DATA_TYPE::~DATA_TYPE()
    {
    }
} // namespace Engine