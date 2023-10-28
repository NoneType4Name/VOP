#include <RHI.hxx>
#include <device.hxx>

namespace Engine
{
    namespace tools
    {
        namespace
        {
            VkInstance _instance { nullptr };
            AppCreateInfo _settings;
            std::vector<VkPhysicalDevice> _aviliable_phDevices {};
            std::vector<VkPhysicalDeviceProperties> _aviliable_phDevicesProperties {};
        } // namespace

        std::vector<VkPhysicalDevice> getAviliableDevices()
        {
            if ( _aviliable_phDevices.empty() )
            {
                uint32_t _c { 0 };
                CHECK_RESULT( vkEnumeratePhysicalDevices( _instance, &_c, nullptr ) );
                _aviliable_phDevices.reserve( _c );
                _aviliable_phDevices.clear();
                CHECK_RESULT( vkEnumeratePhysicalDevices( _instance, &_c, _aviliable_phDevices.data() ) );
            }
            return _aviliable_phDevices;
        }

        std::vector<VkPhysicalDeviceProperties> getAviliableDevicesProperties()
        {
            if ( _aviliable_phDevicesProperties.empty() )
            {
                _aviliable_phDevicesProperties.resize( _aviliable_phDevices.size() );
                for ( size_t i { 0 }; _aviliable_phDevices.size(); i++ )
                    vkGetPhysicalDeviceProperties( _aviliable_phDevices[ i ], &_aviliable_phDevicesProperties[ i ] );
            }

            return _aviliable_phDevicesProperties;
        }

        void createImage( const uint32_t width, const uint32_t height, const VkBufferUsageFlags iUsage, const VkImageTiling tiling, const VkMemoryPropertyFlags mProperties, VkImageCreateInfo ImageCreateInfo, VkImage &Image, VkDeviceMemory &mImage, VkDeviceSize MemoryOffset )
        {
            ImageCreateInfo.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            ImageCreateInfo.imageType     = VK_IMAGE_TYPE_2D;
            ImageCreateInfo.extent.width  = width;
            ImageCreateInfo.extent.height = height;
            ImageCreateInfo.extent.depth  = ImageCreateInfo.extent.depth ? ImageCreateInfo.extent.depth : 1;
            ImageCreateInfo.mipLevels     = ImageCreateInfo.mipLevels ? ImageCreateInfo.mipLevels : 1;
            ImageCreateInfo.arrayLayers   = ImageCreateInfo.arrayLayers ? ImageCreateInfo.arrayLayers : 1;
            ImageCreateInfo.format        = ImageCreateInfo.format ? ImageCreateInfo.format : VK_FORMAT_R8G8B8A8_SRGB;
            ImageCreateInfo.tiling        = tiling;
            ImageCreateInfo.initialLayout = ImageCreateInfo.initialLayout ? ImageCreateInfo.initialLayout : VK_IMAGE_LAYOUT_UNDEFINED;
            ImageCreateInfo.usage         = iUsage;
            ImageCreateInfo.sharingMode   = ImageCreateInfo.sharingMode ? ImageCreateInfo.sharingMode : VK_SHARING_MODE_EXCLUSIVE;
            ImageCreateInfo.samples       = ImageCreateInfo.samples ? ImageCreateInfo.samples : VK_SAMPLE_COUNT_1_BIT;

            CHECK_RESULT( vkCreateImage( getDevice(), &ImageCreateInfo, ALLOCATION_CALLBACK, &Image ) );

            VkMemoryRequirements MemoryRequirements {};
            vkGetImageMemoryRequirements( getDevice(), Image, &MemoryRequirements );

            VkMemoryAllocateInfo MemoryAllocateInfo {};
            MemoryAllocateInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            MemoryAllocateInfo.allocationSize  = MemoryRequirements.size;
            MemoryAllocateInfo.memoryTypeIndex = memoryTypeIndex( MemoryRequirements.memoryTypeBits, mProperties );

            CHECK_RESULT( vkAllocateMemory( getDevice(), &MemoryAllocateInfo, ALLOCATION_CALLBACK, &mImage ) );
            CHECK_RESULT( vkBindImageMemory( getDevice(), Image, mImage, MemoryOffset ) );
        }

        VkImageView createImageView( VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageViewCreateInfo ImageViewCreateInfo )
        {
            VkImageView ImageView;
            ImageViewCreateInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            ImageViewCreateInfo.image                           = image;
            ImageViewCreateInfo.format                          = format;
            ImageViewCreateInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
            ImageViewCreateInfo.subresourceRange.aspectMask     = aspectFlags;
            ImageViewCreateInfo.subresourceRange.baseMipLevel   = ImageViewCreateInfo.subresourceRange.baseMipLevel ? ImageViewCreateInfo.subresourceRange.baseMipLevel : 0;
            ImageViewCreateInfo.subresourceRange.levelCount     = ImageViewCreateInfo.subresourceRange.levelCount ? ImageViewCreateInfo.subresourceRange.levelCount : 1;
            ImageViewCreateInfo.subresourceRange.baseArrayLayer = ImageViewCreateInfo.subresourceRange.baseArrayLayer ? ImageViewCreateInfo.subresourceRange.baseArrayLayer : 0;
            ImageViewCreateInfo.subresourceRange.layerCount     = ImageViewCreateInfo.subresourceRange.layerCount ? ImageViewCreateInfo.subresourceRange.layerCount : 1;

            CHECK_RESULT( vkCreateImageView( getDevice(), &ImageViewCreateInfo, ALLOCATION_CALLBACK, &ImageView ) );
            return ImageView;
        }
    } // namespace tools
} // namespace Engine
