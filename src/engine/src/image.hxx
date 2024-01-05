#pragma once
#ifndef IMAGE_HXX
#    define IMAGE_HXX
#    include <common/globals.hxx>
#    include <common/logging.hxx>
#    include <platform.hxx>

namespace Engine
{
    class image::DATA_TYPE
    {
      public:
        DATA_TYPE();
        // image( types::device device, VkExtent3D extend, const VkBufferUsageFlags iUsage, const VkImageTiling tiling, const VkMemoryPropertyFlags mProperties, VkImageAspectFlags aspect, VkFormat format, VkImageCreateInfo ImageCreateInfo = {} );
        // image( types::device device, VkExtent3D extend, VkImage image, VkImageAspectFlags aspect, VkFormat format, uint32_t mipLevels, uint32_t arrayLayers );
        ~DATA_TYPE();
        types::device device { nullptr };
        VkExtent3D extend;
        VkImageSubresourceLayers subresource;
        uint32_t memoryIndex { 0 };
        VkImage handle { nullptr };
        VkImageView view { nullptr };
        VkDeviceMemory memory { nullptr };
        types::image parent;
    };
} // namespace Engine
#endif