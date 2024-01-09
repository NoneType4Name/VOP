#include <swapchain.hxx>
#include <device.hxx>
#include <surface.hxx>
// #include <image.hxx>
#include <EHI.hxx>

namespace Engine
{
    // namespace
    // {
    //     VkSwapchainKHR _swapchain { nullptr };
    //     VkSurfaceFormatKHR _swapchainSurfaceFormat { VK_FORMAT_MAX_ENUM };
    //     VkPresentModeKHR _swapchainSurfacePresentMode { VK_PRESENT_MODE_MAX_ENUM_KHR };
    //     SwapchainProperties _swapchainProperties {};
    //     VkFormat _depthImageFormat { VK_FORMAT_MAX_ENUM };
    //     std::vector<SwapchainImage> _swapchainImages {};
    //     uint32_t _imageIndex { 0 };
    //     uint32_t _semaphoreIndex { 0 };
    //     VkSwapchainCreateInfoKHR _swapchainCreateInfo {};
    // } // namespace
    // SwapchainProperties getSwapchainProperties()
    // {
    //     auto device { tools::getPhysicalDevice() };
    //     auto surface { tools::getSurface() };
    //     SwapchainProperties Properties;
    //     vkGetPhysicalDeviceSurfaceCapabilitiesKHR( device, surface, &Properties.Capabilities );
    //     uint32_t formatsCount { 0 };
    //     vkGetPhysicalDeviceSurfaceFormatsKHR( device, surface, &formatsCount, nullptr );
    //     // if (formatsCount)
    //     Properties.Format.resize( formatsCount );
    //     vkGetPhysicalDeviceSurfaceFormatsKHR( device, surface, &formatsCount, Properties.Format.data() );
    //     uint32_t PresentModesCount { 0 };
    //     vkGetPhysicalDeviceSurfacePresentModesKHR( device, surface, &PresentModesCount, nullptr );
    //     Properties.PresentModes.resize( PresentModesCount );
    //     vkGetPhysicalDeviceSurfacePresentModesKHR( device, surface, &PresentModesCount, Properties.PresentModes.data() );
    //     return Properties;
    // }

    // VkSurfaceFormatKHR getSwapchainSurfaceFormat()
    // {
    //     if ( _swapchainSurfaceFormat.format != VK_FORMAT_MAX_ENUM )
    //         return _swapchainSurfaceFormat;
    //     VkSurfaceFormatKHR SurfaceFormat { _swapchainProperties.Format[ 0 ] };
    //     for ( const auto &format : _swapchainProperties.Format )
    //     {
    //         if ( format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR ) SurfaceFormat = format;
    //         break;
    //     }
    //     return SurfaceFormat;
    // }

    // VkPresentModeKHR getSwapchainSurfacePresentMode()
    // {
    //     if ( _swapchainSurfacePresentMode != VK_PRESENT_MODE_MAX_ENUM_KHR )
    //         return _swapchainSurfacePresentMode;

    //     VkPresentModeKHR SurfacePresentMode { VK_PRESENT_MODE_FIFO_KHR };
    //     for ( const auto &mode : _swapchainProperties.PresentModes )
    //     {
    //         if ( mode == VK_PRESENT_MODE_MAILBOX_KHR ) SurfacePresentMode = mode;
    //         break;
    //     }
    //     return SurfacePresentMode;
    // }

    // VkFormat getSwapchainDepthImageFormat()
    // {
    //     if ( _depthImageFormat != VK_FORMAT_MAX_ENUM )
    //         return _depthImageFormat;
    //     VkFormat Format { VK_FORMAT_MAX_ENUM };
    //     for ( auto format : std::vector<VkFormat> { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT } )
    //     {
    //         VkFormatProperties FormatProperties {};
    //         vkGetPhysicalDeviceFormatProperties( tools::getPhysicalDevice(), format, &FormatProperties );
    //         if ( ( FormatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT ) == VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT )
    //         {
    //             Format = format;
    //             break;
    //         }
    //     }
    //     return Format;
    // }

    swapchain::swapchain( types::device device, window::types::window window )
    {
        *const_cast<std::unique_ptr<DATA_TYPE> *>( &data ) = std::make_unique<DATA_TYPE>( this, device, window );
        device->data->regSwapchain( this );
        data->device->data->swapchains.emplace( this );
        data->window->data->swapchains.emplace( this );
        // std::vector<void *> swapchainData;
        // std::vector<void *> pData;
        // data->window->data->instance->data->setup->swapchainInfo( this, data->createInfo, swapchainData, data->window->data->instance->data->userPointer );
        // data->setup( this, data->createInfo, pData, data->window->data->instance->data->userPointer );
        // CHECK_RESULT( vkCreateSwapchainKHR( data->device->data->handle, &data->createInfo, ALLOCATION_CALLBACK, &data->swapchain ) );
        // data->setupImgs();
    }

    swapchain::~swapchain()
    {
        vkDestroySwapchainKHR( data->device->data->handle, data->handle, ALLOCATION_CALLBACK );
        data->device->data->swapchains.erase( this );
        data->window->data->swapchains.erase( this );
    };

    void swapchain::setup()
    {
        VkSwapchainCreateInfoKHR createInfo {};
        VkSurfaceFormatKHR SurfaceFormat { data->properties.formats[ 0 ] };
        for ( const auto &format : data->properties.formats )
        {
            VkFormatProperties properties;
            vkGetPhysicalDeviceFormatProperties( data->device->data->description->data->phDevice, format.format, &properties );
            if ( format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR ) SurfaceFormat = format;
        }

        data->presentMode = VK_PRESENT_MODE_FIFO_KHR;
        for ( const auto &mode : data->properties.presentModes )
        {
            if ( mode == VK_PRESENT_MODE_MAILBOX_KHR ) data->presentMode = mode;
        }

        data->depthImageFormat = data->device->data->formatPriority( { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT }, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT );
        int width, height;
        glfwGetFramebufferSize( data->window->data->window, &width, &height );
        createInfo.imageUsage         = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        createInfo.imageFormat        = SurfaceFormat.format;
        createInfo.imageExtent.width  = std::clamp( static_cast<uint32_t>( width ), data->properties.capabilities.minImageExtent.width, data->properties.capabilities.maxImageExtent.width );
        createInfo.imageExtent.height = std::clamp( static_cast<uint32_t>( height ), data->properties.capabilities.minImageExtent.height, data->properties.capabilities.maxImageExtent.height );
        createInfo.minImageCount      = data->properties.capabilities.minImageCount;
        createInfo.clipped            = VK_FALSE;
        createInfo.oldSwapchain       = data->handle;
        createInfo.preTransform       = data->properties.capabilities.currentTransform;
        createInfo.compositeAlpha     = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        data->create( createInfo );
    }

    swapchain::DATA_TYPE::properties_T::properties_T( types::device device, window::types::window window )
    {
        uint32_t c;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR( device->data->description->data->phDevice, window->data->surface, &capabilities );
        glfwSetWindowSizeLimits( window->data->window, capabilities.minImageExtent.width, capabilities.minImageExtent.height, capabilities.maxImageExtent.width, capabilities.maxImageExtent.height );
        vkGetPhysicalDeviceSurfaceFormatsKHR( device->data->description->data->phDevice, window->data->surface, &c, nullptr );
        formats.resize( c );
        vkGetPhysicalDeviceSurfaceFormatsKHR( device->data->description->data->phDevice, window->data->surface, &c, formats.data() );
        vkGetPhysicalDeviceSurfacePresentModesKHR( device->data->description->data->phDevice, window->data->surface, &c, nullptr );
        presentModes.resize( c );
        vkGetPhysicalDeviceSurfacePresentModesKHR( device->data->description->data->phDevice, window->data->surface, &c, presentModes.data() );
    }

    void swapchain::DATA_TYPE::create( VkSwapchainCreateInfoKHR createInfo )
    {
        createInfo.sType   = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = window->data->surface;
        if ( createInfo.minImageCount < properties.capabilities.minImageCount )
            createInfo.minImageCount = properties.capabilities.minImageCount;
        format.format                         = createInfo.imageFormat;
        format.colorSpace                     = createInfo.imageColorSpace;
        presentMode                           = createInfo.presentMode;
        createInfo.imageExtent.width          = std::clamp( static_cast<uint32_t>( createInfo.imageExtent.width ), properties.capabilities.minImageExtent.width, properties.capabilities.maxImageExtent.width );
        createInfo.imageExtent.height         = std::clamp( static_cast<uint32_t>( createInfo.imageExtent.height ), properties.capabilities.minImageExtent.height, properties.capabilities.maxImageExtent.height );
        properties.capabilities.currentExtent = createInfo.imageExtent;
        if ( !createInfo.imageArrayLayers )
            createInfo.imageArrayLayers = 1;
        if ( device->data->queuesSet.count() == device->data->queuesSet.getUniqueIndecies().size() )
        {
            std::vector<uint32_t> indecies {};
            indecies.reserve( device->data->queuesSet.getUniqueIndecies().size() );
            device->data->queuesSet.getUniqueIndecies();
            for ( const auto &ind : device->data->queuesSet.getUniqueIndecies() )
                indecies.push_back( ind.first );
            createInfo.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = static_cast<uint32_t>( indecies.size() );
            createInfo.pQueueFamilyIndices   = indecies.data();
        }
        else
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }
        createInfo.oldSwapchain = handle;
        vkCreateSwapchainKHR( device->data->handle, &createInfo, ALLOCATION_CALLBACK, &handle );
        uint32_t c;
        vkGetSwapchainImagesKHR( device->data->handle, handle, &c, nullptr );
        std::vector<VkImage> imgs { c };
        parent->images.resize( c );
        vkGetSwapchainImagesKHR( device->data->handle, handle, &c, imgs.data() );
        VkSemaphoreCreateInfo semaphoreInfo {};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        for ( size_t i { 0 }; i < imgs.size(); i++ )
        {
            parent->images[ i ].image = new image { device, nullptr, { .image = imgs[ i ], .viewType = VK_IMAGE_VIEW_TYPE_2D, .format = format.format, .subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 } } };
            CHECK_RESULT( vkCreateSemaphore( device->data->handle, &semaphoreInfo, ALLOCATION_CALLBACK, &parent->images[ i ].isAvailable ) );
            CHECK_RESULT( vkCreateSemaphore( device->data->handle, &semaphoreInfo, ALLOCATION_CALLBACK, &parent->images[ i ].isRendered ) );
        }
    }

    swapchain::DATA_TYPE::DATA_TYPE( types::swapchain parent, types::device device, window::types::window window ) :
        parent { parent }, properties( device, window )
    {
        this->device = device;
        this->window = window;
    }

    swapchain::DATA_TYPE::~DATA_TYPE()
    {
        for ( auto &img : parent->images )
        {
            vkDestroySemaphore( device->data->handle, img.isAvailable, ALLOCATION_CALLBACK );
            vkDestroySemaphore( device->data->handle, img.isRendered, ALLOCATION_CALLBACK );
            delete img.image;
        }
    }

    // uint32_t AcquireImageIndex( VkSemaphore &semaphore )
    // {
    //     _semaphoreIndex = ( _semaphoreIndex + 1 ) % _swapchainImages.size();
    //     CHECK_RESULT( vkAcquireNextImageKHR( tools::getDevice(), _swapchain, UINT32_MAX, _swapchainImages[ _semaphoreIndex ].isAvailable, nullptr, &_imageIndex ) )
    //     semaphore = _swapchainImages[ _imageIndex ].isAvailable;
    //     return _imageIndex;
    // }

    // void swapchainPresent( VkSemaphore *semaphore )
    // {
    //     VkPresentInfoKHR PresentInfo {};
    //     PresentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    //     PresentInfo.waitSemaphoreCount = 1;
    //     PresentInfo.pWaitSemaphores    = semaphore;
    //     PresentInfo.swapchainCount     = 1;
    //     PresentInfo.pSwapchains        = &_swapchain;
    //     PresentInfo.pImageIndices      = &_imageIndex;
    //     vkQueuePresentKHR( tools::getQueues().present.GetHandle(), &PresentInfo );
    // }
} // namespace Engine