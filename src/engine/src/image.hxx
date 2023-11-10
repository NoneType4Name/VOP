#pragma once
#include <common/globals.hxx>
#include <common/logging.hxx>
#include <platform.hxx>
#include <EHI.hxx>

namespace Engine
{
    class image
    {
      private:
        class data;
        VkImage handler { nullptr };
        VkImageView view { nullptr };
        VkDeviceMemory memory { nullptr };

      public:
        image() = default;
        image( VkExtent3D extend, const VkBufferUsageFlags iUsage, const VkImageTiling tiling, const VkMemoryPropertyFlags mProperties, VkImageAspectFlags aspect, VkFormat format, VkImageCreateInfo ImageCreateInfo = {} );
        image( VkExtent3D extend, VkImage &image, VkImageAspectFlags aspect, VkFormat format, uint32_t mipLevels, uint32_t arrayLayers );
        image( const uint32_t width, const uint32_t height, const VkBufferUsageFlags iUsage, const VkImageTiling tiling, const VkMemoryPropertyFlags mProperties, VkImageAspectFlags aspect, VkFormat format, VkImageCreateInfo ImageCreateInfo = {} );
        std::unique_ptr<data> data;
        imageID getID() const;
        VkImage getImage() const;
        VkImageView getImageView() const;
        VkDeviceMemory getImageMemory() const;
        ~image();
    };
    typedef uint32_t as;
    DEFINE_HANDLE( image );
    namespace tools
    {
    } // namespace tools
} // namespace Engine