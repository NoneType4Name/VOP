#pragma once
#include <common/globals.hxx>
#include <common/logging.hxx>
#include <platform.hxx>

namespace Engine
{
    namespace tools
    {
        typedef uint64_t imageID;
        class image
        {
          public:
            image() = default;
            image( VkExtent3D extend, const VkBufferUsageFlags iUsage, const VkImageTiling tiling, const VkMemoryPropertyFlags mProperties, VkImageAspectFlags aspect, VkFormat format, VkImageCreateInfo ImageCreateInfo = {} );
            image( VkExtent3D extend, VkImage &image, VkImageAspectFlags aspect, VkFormat format, uint32_t mipLevels, uint32_t arrayLayers );
            image( const uint32_t width, const uint32_t height, const VkBufferUsageFlags iUsage, const VkImageTiling tiling, const VkMemoryPropertyFlags mProperties, VkImageAspectFlags aspect, VkFormat format, VkImageCreateInfo ImageCreateInfo = {} );
            imageID getID() const;
            VkImage getImage() const;
            VkImageView getImageView() const;
            VkDeviceMemory getImageMemory() const;
            ~image();

          private:
            imageID id { 0 };
            VkImage handler { nullptr };
            VkImageView view { nullptr };
            VkDeviceMemory memory { nullptr };
        };
        image *getImage( imageID id );
    } // namespace tools
} // namespace Engine