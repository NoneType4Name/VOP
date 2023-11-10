#include <device.hxx>
#include <EHI.hxx>
#include <swapchain.hxx>

namespace Engine
{
    namespace tools
    {
    } // namespace tools
    namespace
    {
        VkSwapchainKHR _swapchain { nullptr };
        VkSurfaceFormatKHR _swapchainSurfaceFormat { VK_FORMAT_MAX_ENUM };
        VkPresentModeKHR _swapchainSurfacePresentMode { VK_PRESENT_MODE_MAX_ENUM_KHR };
        SwapchainProperties _swapchainProperties {};
        VkFormat _depthImageFormat { VK_FORMAT_MAX_ENUM };
        std::vector<SwapchainImage> _swapchainImages {};
        uint32_t _imageIndex { 0 };
        uint32_t _semaphoreIndex { 0 };
        VkSwapchainCreateInfoKHR _swapchainCreateInfo {};
    } // namespace
    SwapchainProperties getSwapchainProperties()
    {
        auto device { tools::getPhysicalDevice() };
        auto surface { tools::getSurface() };
        SwapchainProperties Properties;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR( device, surface, &Properties.Capabilities );
        uint32_t formatsCount { 0 };
        vkGetPhysicalDeviceSurfaceFormatsKHR( device, surface, &formatsCount, nullptr );
        // if (formatsCount)
        Properties.Format.resize( formatsCount );
        vkGetPhysicalDeviceSurfaceFormatsKHR( device, surface, &formatsCount, Properties.Format.data() );
        uint32_t PresentModesCount { 0 };
        vkGetPhysicalDeviceSurfacePresentModesKHR( device, surface, &PresentModesCount, nullptr );
        Properties.PresentModes.resize( PresentModesCount );
        vkGetPhysicalDeviceSurfacePresentModesKHR( device, surface, &PresentModesCount, Properties.PresentModes.data() );
        return Properties;
    }

    VkSurfaceFormatKHR getSwapchainSurfaceFormat()
    {
        if ( _swapchainSurfaceFormat.format != VK_FORMAT_MAX_ENUM )
            return _swapchainSurfaceFormat;
        VkSurfaceFormatKHR SurfaceFormat { _swapchainProperties.Format[ 0 ] };
        for ( const auto &format : _swapchainProperties.Format )
        {
            if ( format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR ) SurfaceFormat = format;
            break;
        }
        return SurfaceFormat;
    }

    VkPresentModeKHR getSwapchainSurfacePresentMode()
    {
        if ( _swapchainSurfacePresentMode != VK_PRESENT_MODE_MAX_ENUM_KHR )
            return _swapchainSurfacePresentMode;

        VkPresentModeKHR SurfacePresentMode { VK_PRESENT_MODE_FIFO_KHR };
        for ( const auto &mode : _swapchainProperties.PresentModes )
        {
            if ( mode == VK_PRESENT_MODE_MAILBOX_KHR ) SurfacePresentMode = mode;
            break;
        }
        return SurfacePresentMode;
    }

    VkFormat getSwapchainDepthImageFormat()
    {
        if ( _depthImageFormat != VK_FORMAT_MAX_ENUM )
            return _depthImageFormat;
        VkFormat Format { VK_FORMAT_MAX_ENUM };
        for ( auto format : std::vector<VkFormat> { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT } )
        {
            VkFormatProperties FormatProperties {};
            vkGetPhysicalDeviceFormatProperties( tools::getPhysicalDevice(), format, &FormatProperties );
            if ( ( FormatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT ) == VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT )
            {
                Format = format;
                break;
            }
        }
        return Format;
    }

    link::link( window::types::window window, types::device device )
    {
        data->window                 = window;
        data->device                 = device;
        _swapchainProperties         = getSwapchainProperties();
        _swapchainSurfaceFormat      = getSwapchainSurfaceFormat();
        _swapchainSurfacePresentMode = getSwapchainSurfacePresentMode();
        _depthImageFormat            = getSwapchainDepthImageFormat();
        glfwSetWindowSizeLimits( data->window->data->window, _swapchainProperties.Capabilities.minImageExtent.width, _swapchainProperties.Capabilities.minImageExtent.height, _swapchainProperties.Capabilities.maxImageExtent.width, _swapchainProperties.Capabilities.maxImageExtent.height );
        VkSwapchainCreateInfoKHR SwapchainCreateInfo {};
        SwapchainCreateInfo.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        SwapchainCreateInfo.surface          = data->window->data->surface;
        SwapchainCreateInfo.minImageCount    = _swapchainProperties.Capabilities.minImageCount; // imgs count
        SwapchainCreateInfo.imageFormat      = _swapchainSurfaceFormat.format;
        SwapchainCreateInfo.imageColorSpace  = _swapchainSurfaceFormat.colorSpace;
        SwapchainCreateInfo.presentMode      = _swapchainSurfacePresentMode;
        SwapchainCreateInfo.imageExtent      = _swapchainProperties.Capabilities.currentExtent;
        SwapchainCreateInfo.imageArrayLayers = 1;
        SwapchainCreateInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        auto queues { data->device->data->queuesSet };
        if ( queues.count() == queues.getUniqueIndecies().size() )
        {
            std::vector<uint32_t> indecies {};
            indecies.reserve( queues.getUniqueIndecies().size() );
            for ( const auto &ind : queues.getUniqueIndecies() )
                indecies.push_back( ind.first );
            SwapchainCreateInfo.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
            SwapchainCreateInfo.queueFamilyIndexCount = static_cast<uint32_t>( indecies.size() );
            SwapchainCreateInfo.pQueueFamilyIndices   = indecies.data();
        }
        else
        {
            SwapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }
        SwapchainCreateInfo.preTransform   = _swapchainProperties.Capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR ? VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR : _swapchainProperties.Capabilities.currentTransform;
        SwapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        SwapchainCreateInfo.clipped        = VK_TRUE;
        SwapchainCreateInfo.oldSwapchain   = VK_NULL_HANDLE;
        CHECK_RESULT( vkCreateSwapchainKHR( data->device->data->device, &SwapchainCreateInfo, ALLOCATION_CALLBACK, &_swapchain ) );
        uint32_t _c { 0 };
        vkGetSwapchainImagesKHR( data->device->data->device, _swapchain, &_c, nullptr );
        std::vector<VkImage> imgs { _c };
        _swapchainImages.resize( _c );
        vkGetSwapchainImagesKHR( data->device->data->device, _swapchain, &_c, imgs.data() );
        VkSemaphoreCreateInfo semaphoreInfo {};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        for ( size_t i { 0 }; i < imgs.size(); i++ )
        {
            _swapchainImages[ i ].image = ( new image( { SwapchainCreateInfo.imageExtent.width, SwapchainCreateInfo.imageExtent.height }, imgs[ i ], VK_IMAGE_ASPECT_COLOR_BIT, SwapchainCreateInfo.imageFormat, 1, SwapchainCreateInfo.imageArrayLayers ) )->getID();
            // CHECK_RESULT( vkCreateImageView( tools::getDevice(), &ImageViewCreateInfo, ALLOCATION_CALLBACK, &_swapchainImages[ i ].view ) );
            CHECK_RESULT( vkCreateSemaphore( data->device->data->device, &semaphoreInfo, ALLOCATION_CALLBACK, &_swapchainImages[ i ].isAvailable ) );
            CHECK_RESULT( vkCreateSemaphore( data->device->data->device, &semaphoreInfo, ALLOCATION_CALLBACK, &_swapchainImages[ i ].isRendered ) );
        }
    }

    void reCreateSwapchain()
    {
        _imageIndex = 0;
        for ( const auto &img : _swapchainImages )
        {
            delete getImage( img.image );
        }
        _swapchainSurfaceFormat.format = VK_FORMAT_MAX_ENUM;
        _swapchainSurfacePresentMode   = VK_PRESENT_MODE_MAX_ENUM_KHR;
        _depthImageFormat              = VK_FORMAT_MAX_ENUM;
        _swapchainProperties           = getSwapchainProperties();
        _swapchainSurfaceFormat        = getSwapchainSurfaceFormat();
        _swapchainSurfacePresentMode   = getSwapchainSurfacePresentMode();
        _depthImageFormat              = getSwapchainDepthImageFormat();
        glfwSetWindowSizeLimits( tools::getWindow(), _swapchainProperties.Capabilities.minImageExtent.width, _swapchainProperties.Capabilities.minImageExtent.height, _swapchainProperties.Capabilities.maxImageExtent.width, _swapchainProperties.Capabilities.maxImageExtent.height );
        VkSwapchainCreateInfoKHR SwapchainCreateInfo {};
        SwapchainCreateInfo.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        SwapchainCreateInfo.surface          = tools::getSurface();
        SwapchainCreateInfo.minImageCount    = _swapchainProperties.Capabilities.minImageCount; // imgs count
        SwapchainCreateInfo.imageFormat      = _swapchainSurfaceFormat.format;
        SwapchainCreateInfo.imageColorSpace  = _swapchainSurfaceFormat.colorSpace;
        SwapchainCreateInfo.presentMode      = _swapchainSurfacePresentMode;
        SwapchainCreateInfo.imageExtent      = _swapchainProperties.Capabilities.currentExtent;
        SwapchainCreateInfo.imageArrayLayers = 1;
        SwapchainCreateInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        auto queues { tools::getQueues() };
        if ( queues.count() == queues.getUniqueIndeciesCount().size() )
        {
            std::vector<uint32_t> indecies {};
            indecies.reserve( queues.getUniqueIndeciesCount().size() );
            for ( const auto &ind : queues.getUniqueIndeciesCount() )
                indecies.push_back( ind.first );
            SwapchainCreateInfo.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
            SwapchainCreateInfo.queueFamilyIndexCount = static_cast<uint32_t>( indecies.size() );
            SwapchainCreateInfo.pQueueFamilyIndices   = indecies.data();
        }
        else
        {
            SwapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }
        SwapchainCreateInfo.preTransform   = _swapchainProperties.Capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR ? VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR : _swapchainProperties.Capabilities.currentTransform;
        SwapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        SwapchainCreateInfo.clipped        = VK_TRUE;
        SwapchainCreateInfo.oldSwapchain   = _swapchain;
        CHECK_RESULT( vkCreateSwapchainKHR( tools::getDevice(), &SwapchainCreateInfo, ALLOCATION_CALLBACK, &_swapchain ) );
        vkDestroySwapchainKHR( tools::getDevice(), SwapchainCreateInfo.oldSwapchain, ALLOCATION_CALLBACK );
        uint32_t _c { 0 };
        vkGetSwapchainImagesKHR( tools::getDevice(), _swapchain, &_c, nullptr );
        std::vector<VkImage> imgs { _c };
        _swapchainImages.resize( _c );
        vkGetSwapchainImagesKHR( tools::getDevice(), _swapchain, &_c, imgs.data() );
        for ( size_t i { 0 }; i < imgs.size(); i++ )
        {
            _swapchainImages[ i ].image = ( new image( { SwapchainCreateInfo.imageExtent.width, SwapchainCreateInfo.imageExtent.height }, imgs[ i ], VK_IMAGE_ASPECT_COLOR_BIT, SwapchainCreateInfo.imageFormat, 1, SwapchainCreateInfo.imageArrayLayers ) )->getID();
        }
    }

    void destroySwapchain()
    {
        _swapchainSurfaceFormat.format = VK_FORMAT_MAX_ENUM;
        _swapchainSurfacePresentMode   = VK_PRESENT_MODE_MAX_ENUM_KHR;
        _depthImageFormat              = VK_FORMAT_MAX_ENUM;
        for ( const auto &img : _swapchainImages )
        {
            vkDestroySemaphore( tools::getDevice(), img.isAvailable, ALLOCATION_CALLBACK );
            vkDestroySemaphore( tools::getDevice(), img.isRendered, ALLOCATION_CALLBACK );
            delete getImage( img.image );
        }
        vkDestroySwapchainKHR( tools::getDevice(), _swapchain, ALLOCATION_CALLBACK );
    }

    uint32_t AcquireImageIndex( VkSemaphore &semaphore )
    {
        _semaphoreIndex = ( _semaphoreIndex + 1 ) % _swapchainImages.size();
        CHECK_RESULT( vkAcquireNextImageKHR( tools::getDevice(), _swapchain, UINT32_MAX, _swapchainImages[ _semaphoreIndex ].isAvailable, nullptr, &_imageIndex ) )
        semaphore = _swapchainImages[ _imageIndex ].isAvailable;
        return _imageIndex;
    }

    void swapchainPresent( VkSemaphore *semaphore )
    {
        VkPresentInfoKHR PresentInfo {};
        PresentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        PresentInfo.waitSemaphoreCount = 1;
        PresentInfo.pWaitSemaphores    = semaphore;
        PresentInfo.swapchainCount     = 1;
        PresentInfo.pSwapchains        = &_swapchain;
        PresentInfo.pImageIndices      = &_imageIndex;
        vkQueuePresentKHR( tools::getQueues().present.GetHandle(), &PresentInfo );
    }
} // namespace Engine