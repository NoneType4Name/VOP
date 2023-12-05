#include <image.hxx>
#include <device.hxx>

namespace Engine
{
    namespace
    {
        std::vector<image *> images;
    }

    image::image( types::device device, VkExtent3D extend, const VkBufferUsageFlags iUsage, const VkImageTiling tiling, const VkMemoryPropertyFlags mProperties, VkImageAspectFlags aspect, VkFormat format, VkImageCreateInfo ImageCreateInfo )
    {
        images.emplace_back( this );
        this->device                  = device;
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

        CHECK_RESULT( vkCreateImage( this->device->data->device, &ImageCreateInfo, ALLOCATION_CALLBACK, &handle ) );
        memoryIndex = this->device->data->setImageMemory( handle, mProperties );

        VkImageViewCreateInfo ImageViewCreateInfo {};
        ImageViewCreateInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        ImageViewCreateInfo.image                           = handle;
        ImageViewCreateInfo.format                          = format;
        ImageViewCreateInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
        ImageViewCreateInfo.components                      = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
        ImageViewCreateInfo.subresourceRange.aspectMask     = aspect;
        ImageViewCreateInfo.subresourceRange.baseMipLevel   = 0;
        ImageViewCreateInfo.subresourceRange.levelCount     = ImageCreateInfo.mipLevels;
        ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        ImageViewCreateInfo.subresourceRange.layerCount     = ImageCreateInfo.arrayLayers;
        CHECK_RESULT( vkCreateImageView( this->device->data->device, &ImageViewCreateInfo, ALLOCATION_CALLBACK, &view ) );
    }

    image::image( types::device device, VkExtent3D extend, VkImage image, VkImageAspectFlags aspect, VkFormat format, uint32_t mipLevels, uint32_t arrayLayers )
    {
        images.emplace_back( this );
        this->device = device;
        handle       = image;
        VkImageViewCreateInfo ImageViewCreateInfo {};
        ImageViewCreateInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        ImageViewCreateInfo.image                           = handle;
        ImageViewCreateInfo.format                          = format;
        ImageViewCreateInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
        ImageViewCreateInfo.components                      = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
        ImageViewCreateInfo.subresourceRange.aspectMask     = aspect;
        ImageViewCreateInfo.subresourceRange.baseMipLevel   = 0;
        ImageViewCreateInfo.subresourceRange.levelCount     = mipLevels;
        ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        ImageViewCreateInfo.subresourceRange.layerCount     = arrayLayers;

        CHECK_RESULT( vkCreateImageView( this->device->data->device, &ImageViewCreateInfo, ALLOCATION_CALLBACK, &view ) );
    }

    image::image( types::device device, const uint32_t width, const uint32_t height, const VkBufferUsageFlags iUsage, const VkImageTiling tiling, const VkMemoryPropertyFlags mProperties, VkImageAspectFlags aspect, VkFormat format, VkImageCreateInfo ImageCreateInfo ) :
        image::image( device, { width, height }, iUsage, tiling, mProperties, aspect, format, ImageCreateInfo )
    {
    }

    image::~image()
    {
        vkDestroyImageView( this->device->data->device, view, ALLOCATION_CALLBACK );
        if ( memory == nullptr )
            return;
        device->data->resetImageMemory( handle, memoryIndex );
        vkDestroyImage( this->device->data->device, handle, ALLOCATION_CALLBACK );
    }

    namespace tools
    {
    } // namespace tools
} // namespace Engine