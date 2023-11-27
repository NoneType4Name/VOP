#pragma once
#ifndef IMAGE_HXX
#    define IMAGE_HXX
#    include <common/globals.hxx>
#    include <common/logging.hxx>
#    include <platform.hxx>

namespace Engine
{
    class image
    {
      public:
        image() = default;
        image( types::device device, VkExtent3D extend, const VkBufferUsageFlags iUsage, const VkImageTiling tiling, const VkMemoryPropertyFlags mProperties, VkImageAspectFlags aspect, VkFormat format, VkImageCreateInfo ImageCreateInfo = {} );
        image( types::device device, VkExtent3D extend, VkImage image, VkImageAspectFlags aspect, VkFormat format, uint32_t mipLevels, uint32_t arrayLayers );
        image( types::device device, const uint32_t width, const uint32_t height, const VkBufferUsageFlags iUsage, const VkImageTiling tiling, const VkMemoryPropertyFlags mProperties, VkImageAspectFlags aspect, VkFormat format, VkImageCreateInfo ImageCreateInfo = {} );
        ~image();
        types::device device { nullptr };
        VkImage handler { nullptr };
        VkImageView view { nullptr };
        VkDeviceMemory memory { nullptr };
    };
    namespace tools
    {
    } // namespace tools
} // namespace Engine
#endif