#include <image.hxx>

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
    }

    image::image( types::device device, types::image image, VkImageViewCreateInfo ImageViewCreateInfo )
    {
        DEFINE_DATA_FIELD( device, image, ImageViewCreateInfo );
        ImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        CHECK_RESULT( vkCreateImageView( data->device->data->handle, &ImageViewCreateInfo, ALLOCATION_CALLBACK, &data->view ) );
    }

    image::~image()
    {
        vkDestroyImageView( data->device->data->handle, data->view, ALLOCATION_CALLBACK );
        if ( data->ImageViewInfo.image )
            vkDestroyImage( data->device->data->handle, data->ImageViewInfo.image, ALLOCATION_CALLBACK );
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
} // namespace Engine