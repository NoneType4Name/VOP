#include <device.hxx>
#include <RHI.hxx>
#include <swapchain.hxx>
#define IMAGES_COUNT 1

namespace Engine
{
    namespace tools
    {
        struct SwapchainProperties
        {
            VkSurfaceCapabilitiesKHR Capabilities;
            std::vector<VkSurfaceFormatKHR> Format;
            std::vector<VkPresentModeKHR> PresentModes;
        };

        struct SwapchainImage
        {
            VkImage image;
            VkImageView view;
        };

        namespace
        {
            VkSwapchainKHR _swapchain{ nullptr };
            VkSurfaceFormatKHR _swapchainSurfaceFormat{ VK_FORMAT_UNDEFINED };
            VkPresentModeKHR _swapchainSurfacePresentMode;
            SwapchainProperties _swapchainProperties{};
            VkFormat _depthImageFormat{ VK_FORMAT_UNDEFINED };
            std::vector<SwapchainImage> _swapchainImages{};
        } // namespace
        SwapchainProperties
        getSwapchainProperties()
        {
            auto device{ tools::getPhysicalDevice() };
            auto surface{ tools::getSurface() };
            SwapchainProperties Properties;
            vkGetPhysicalDeviceSurfaceCapabilitiesKHR( device, surface, &Properties.Capabilities );
            uint32_t formatsCount{ 0 };
            vkGetPhysicalDeviceSurfaceFormatsKHR( device, surface, &formatsCount, nullptr );
            // if (formatsCount)
            Properties.Format.resize( formatsCount );
            vkGetPhysicalDeviceSurfaceFormatsKHR( device, surface, &formatsCount, Properties.Format.data() );
            uint32_t PresentModesCount{ 0 };
            vkGetPhysicalDeviceSurfacePresentModesKHR( device, surface, &PresentModesCount, nullptr );
            Properties.PresentModes.resize( PresentModesCount );
            vkGetPhysicalDeviceSurfacePresentModesKHR( device, surface, &PresentModesCount, Properties.PresentModes.data() );
            return Properties;
        }

        VkSurfaceFormatKHR getSwapchainSurfaceFormat()
        {
            VkSurfaceFormatKHR SurfaceFormat{ _swapchainProperties.Format[ 0 ] };
            for( const auto &format : _swapchainProperties.Format )
            {
                if( format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR ) SurfaceFormat = format;
                break;
            }
            return SurfaceFormat;
        }

        VkPresentModeKHR getSwapchainSurfacePresentMode()
        {
            VkPresentModeKHR SurfacePresentMode{ VK_PRESENT_MODE_FIFO_KHR };
            for( const auto &mode : _swapchainProperties.PresentModes )
            {
                if( mode == VK_PRESENT_MODE_MAILBOX_KHR ) SurfacePresentMode = mode;
                break;
            }
            return SurfacePresentMode;
        }

        VkFormat getSwapchainDepthImageFormat()
        {
            VkFormat Format{ VK_FORMAT_UNDEFINED };
            for( auto format : std::vector<VkFormat>{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT } )
            {
                VkFormatProperties FormatProperties{};
                vkGetPhysicalDeviceFormatProperties( tools::getPhysicalDevice(), format, &FormatProperties );
                if( ( FormatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT ) == VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT )
                {
                    Format = format;
                    break;
                }
            }
            return Format;
        }

        void createSwapchain()
        {
            _swapchainProperties         = getSwapchainProperties();
            _swapchainSurfaceFormat      = getSwapchainSurfaceFormat();
            _swapchainSurfacePresentMode = getSwapchainSurfacePresentMode();
            _depthImageFormat            = getSwapchainDepthImageFormat();
            VkSwapchainCreateInfoKHR SwapchainCreateInfo{};
            SwapchainCreateInfo.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
            SwapchainCreateInfo.surface          = tools::getSurface();
            SwapchainCreateInfo.minImageCount    = _swapchainProperties.Capabilities.minImageCount;
            SwapchainCreateInfo.imageFormat      = _swapchainSurfaceFormat.format;
            SwapchainCreateInfo.imageColorSpace  = _swapchainSurfaceFormat.colorSpace;
            SwapchainCreateInfo.presentMode      = _swapchainSurfacePresentMode;
            SwapchainCreateInfo.imageExtent      = _swapchainProperties.Capabilities.currentExtent;
            SwapchainCreateInfo.imageArrayLayers = 1;
            SwapchainCreateInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
            auto queues{ tools::getQueues() };
            if( queues.count() == queues.getUniqueIndeciesCount().size() )
            {
                std::vector<uint32_t> indecies{};
                indecies.reserve( queues.getUniqueIndeciesCount().size() );
                for( const auto &ind : queues.getUniqueIndeciesCount() )
                    indecies.push_back( ind.first );
                SwapchainCreateInfo.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
                SwapchainCreateInfo.queueFamilyIndexCount = static_cast<uint32_t>( indecies.size() );
                SwapchainCreateInfo.pQueueFamilyIndices   = indecies.data();
            }
            else
            {
                SwapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            }
            SwapchainCreateInfo.preTransform   = _swapchainProperties.Capabilities.currentTransform;
            SwapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
            SwapchainCreateInfo.clipped        = VK_TRUE;
            SwapchainCreateInfo.oldSwapchain   = VK_NULL_HANDLE;
            CHECK_RESULT( vkCreateSwapchainKHR( tools::getDevice(), &SwapchainCreateInfo, nullptr, &_swapchain ) );
            uint32_t _c{ 0 };
            vkGetSwapchainImagesKHR( tools::getDevice(), _swapchain, &_c, nullptr );
            std::vector<VkImage> imgs{ _c };
            vkGetSwapchainImagesKHR( tools::getDevice(), _swapchain, &_c, imgs.data() );
            for( size_t i{ 0 }; i < imgs.size(); i++ )
            {
                VkImageViewCreateInfo ImageViewCreateInfo           = {};
                ImageViewCreateInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                ImageViewCreateInfo.format                          = _swapchainSurfaceFormat.format;
                ImageViewCreateInfo.components                      = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
                ImageViewCreateInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
                ImageViewCreateInfo.subresourceRange.baseMipLevel   = 0;
                ImageViewCreateInfo.subresourceRange.levelCount     = 1;
                ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
                ImageViewCreateInfo.subresourceRange.layerCount     = 1;
                ImageViewCreateInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
                ImageViewCreateInfo.flags                           = 0;
                ImageViewCreateInfo.image = _swapchainImages[ i ].image = imgs[ i ];
                CHECK_RESULT( vkCreateImageView( tools::getDevice(), &ImageViewCreateInfo, nullptr, &_swapchainImages[ i ].view ) );
            }
        }

        void reCreateSwapchain();
        void destroySwapchain()
        {
            for( const auto &img : _swapchainImages )
                vkDestroyImageView( tools::getDevice(), img.view, ALLOCATION_CALLBACK );
            vkDestroySwapchainKHR( tools::getDevice(), _swapchain, ALLOCATION_CALLBACK );
        }
    } // namespace tools
} // namespace Engine