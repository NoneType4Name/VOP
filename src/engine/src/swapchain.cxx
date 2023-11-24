#include <swapchain.hxx>
#include <device.hxx>
#include <surface.hxx>
#include <image.hxx>
#include <EHI.hxx>

namespace Engine
{
    namespace tools
    {
    } // namespace tools
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

    link::link() = default;
    link::link( window::types::window window, types::device device ) :
        data { new DATA_TYPE { window, device } }
    {
        std::vector<void *> pData;
        std::vector<void *> pData1;
        data->window->data->instance->data->setup->swapchain( this, data->createInfo, pData1, data->window->data->instance->data->userPointer );
        data->setup( this, data->createInfo, pData, data->window->data->instance->data->userPointer );
        CHECK_RESULT( vkCreateSwapchainKHR( data->device->data->device, &data->createInfo, ALLOCATION_CALLBACK, &data->swapchain ) );
        data->setupImgs();
    }

    link::~link()
    {
        vkDestroySwapchainKHR( data->device->data->device, data->swapchain, ALLOCATION_CALLBACK );
    };

    link::DATA_TYPE::properties_T::properties_T( window::types::window window, types::device device )
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

    void InstanceSetup::swapchain( types::link swapchain, VkSwapchainCreateInfoKHR &createInfo, std::vector<void *> &dataPointer, void *userPoiner )
    {
        VkSurfaceFormatKHR SurfaceFormat { swapchain->data->properties.formats[ 0 ] };
        for ( const auto &format : swapchain->data->properties.formats )
        {
            if ( format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR ) SurfaceFormat = format;
        }

        swapchain->data->presentMode = VK_PRESENT_MODE_FIFO_KHR;
        for ( const auto &mode : swapchain->data->properties.presentModes )
        {
            if ( mode == VK_PRESENT_MODE_MAILBOX_KHR ) swapchain->data->presentMode = mode;
        }
        int width, height;
        glfwGetFramebufferSize( swapchain->data->window->data->window, &width, &height );
        createInfo.imageUsage         = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        createInfo.imageFormat        = SurfaceFormat.format;
        createInfo.imageExtent.width  = std::clamp( static_cast<uint32_t>( width ), swapchain->data->properties.capabilities.minImageExtent.width, swapchain->data->properties.capabilities.maxImageExtent.width );
        createInfo.imageExtent.height = std::clamp( static_cast<uint32_t>( height ), swapchain->data->properties.capabilities.minImageExtent.height, swapchain->data->properties.capabilities.maxImageExtent.height );
        createInfo.minImageCount      = swapchain->data->properties.capabilities.minImageCount;
        createInfo.clipped            = VK_FALSE;
        createInfo.oldSwapchain       = swapchain->data->swapchain;
        createInfo.preTransform       = swapchain->data->properties.capabilities.currentTransform;
        createInfo.compositeAlpha     = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    }

    void link::DATA_TYPE::setup( types::link link, VkSwapchainCreateInfoKHR &createInfo, std::vector<void *> &dataPointer, void *userPoiner )
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
    }

    void link::DATA_TYPE::setupImgs()
    {
        uint32_t c;
        vkGetSwapchainImagesKHR( device->data->device, swapchain, &c, nullptr );
        std::vector<VkImage> imgs { c };
        images.resize( c );
        vkGetSwapchainImagesKHR( device->data->device, swapchain, &c, imgs.data() );
        VkSemaphoreCreateInfo semaphoreInfo {};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        for ( size_t i { 0 }; i < imgs.size(); i++ )
        {
            images[ i ].image.reset( new image( device, { createInfo.imageExtent.width, createInfo.imageExtent.height }, imgs[ i ], VK_IMAGE_ASPECT_COLOR_BIT, createInfo.imageFormat, 1, createInfo.imageArrayLayers ) );
            CHECK_RESULT( vkCreateSemaphore( device->data->device, &semaphoreInfo, ALLOCATION_CALLBACK, &images[ i ].isAvailable ) );
            CHECK_RESULT( vkCreateSemaphore( device->data->device, &semaphoreInfo, ALLOCATION_CALLBACK, &images[ i ].isRendered ) );
        }
    }

    link::DATA_TYPE::DATA_TYPE( window::types::window window, types::device device ) :
        properties( window, device )
    {
        this->window = window;
        this->device = device;
    }

    link::DATA_TYPE::~DATA_TYPE()
    {
        for ( auto &img : images )
        {
            vkDestroySemaphore( device->data->device, img.isAvailable, ALLOCATION_CALLBACK );
            vkDestroySemaphore( device->data->device, img.isRendered, ALLOCATION_CALLBACK );
        }
    }

    // void reCreateSwapchain()
    // {
    //     _imageIndex = 0;
    //     for ( const auto &img : _swapchainImages )
    //     {
    //         delete getImage( img.image );
    //     }
    //     _swapchainSurfaceFormat.format = VK_FORMAT_MAX_ENUM;
    //     _swapchainSurfacePresentMode   = VK_PRESENT_MODE_MAX_ENUM_KHR;
    //     _depthImageFormat              = VK_FORMAT_MAX_ENUM;
    //     _swapchainProperties           = getSwapchainProperties();
    //     _swapchainSurfaceFormat        = getSwapchainSurfaceFormat();
    //     _swapchainSurfacePresentMode   = getSwapchainSurfacePresentMode();
    //     _depthImageFormat              = getSwapchainDepthImageFormat();
    //     glfwSetWindowSizeLimits( tools::getWindow(), _swapchainProperties.Capabilities.minImageExtent.width, _swapchainProperties.Capabilities.minImageExtent.height, _swapchainProperties.Capabilities.maxImageExtent.width, _swapchainProperties.Capabilities.maxImageExtent.height );
    //     VkSwapchainCreateInfoKHR SwapchainCreateInfo {};
    //     SwapchainCreateInfo.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    //     SwapchainCreateInfo.surface          = tools::getSurface();
    //     SwapchainCreateInfo.minImageCount    = _swapchainProperties.Capabilities.minImageCount; // imgs count
    //     SwapchainCreateInfo.imageFormat      = _swapchainSurfaceFormat.format;
    //     SwapchainCreateInfo.imageColorSpace  = _swapchainSurfaceFormat.colorSpace;
    //     SwapchainCreateInfo.presentMode      = _swapchainSurfacePresentMode;
    //     SwapchainCreateInfo.imageExtent      = _swapchainProperties.Capabilities.currentExtent;
    //     SwapchainCreateInfo.imageArrayLayers = 1;
    //     SwapchainCreateInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    //     auto queues { tools::getQueues() };
    //     if ( queues.count() == queues.getUniqueIndeciesCount().size() )
    //     {
    //         std::vector<uint32_t> indecies {};
    //         indecies.reserve( queues.getUniqueIndeciesCount().size() );
    //         for ( const auto &ind : queues.getUniqueIndeciesCount() )
    //             indecies.push_back( ind.first );
    //         SwapchainCreateInfo.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
    //         SwapchainCreateInfo.queueFamilyIndexCount = static_cast<uint32_t>( indecies.size() );
    //         SwapchainCreateInfo.pQueueFamilyIndices   = indecies.data();
    //     }
    //     else
    //     {
    //         SwapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    //     }
    //     SwapchainCreateInfo.preTransform   = _swapchainProperties.Capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR ? VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR : _swapchainProperties.Capabilities.currentTransform;
    //     SwapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    //     SwapchainCreateInfo.clipped        = VK_TRUE;
    //     SwapchainCreateInfo.oldSwapchain   = _swapchain;
    //     CHECK_RESULT( vkCreateSwapchainKHR( tools::getDevice(), &SwapchainCreateInfo, ALLOCATION_CALLBACK, &_swapchain ) );
    //     vkDestroySwapchainKHR( tools::getDevice(), SwapchainCreateInfo.oldSwapchain, ALLOCATION_CALLBACK );
    //     uint32_t _c { 0 };
    //     vkGetSwapchainImagesKHR( tools::getDevice(), _swapchain, &_c, nullptr );
    //     std::vector<VkImage> imgs { _c };
    //     _swapchainImages.resize( _c );
    //     vkGetSwapchainImagesKHR( tools::getDevice(), _swapchain, &_c, imgs.data() );
    //     for ( size_t i { 0 }; i < imgs.size(); i++ )
    //     {
    //         _swapchainImages[ i ].image = ( new image( { SwapchainCreateInfo.imageExtent.width, SwapchainCreateInfo.imageExtent.height }, imgs[ i ], VK_IMAGE_ASPECT_COLOR_BIT, SwapchainCreateInfo.imageFormat, 1, SwapchainCreateInfo.imageArrayLayers ) )->getID();
    //     }
    // }

    // void destroySwapchain()
    // {
    //     _swapchainSurfaceFormat.format = VK_FORMAT_MAX_ENUM;
    //     _swapchainSurfacePresentMode   = VK_PRESENT_MODE_MAX_ENUM_KHR;
    //     _depthImageFormat              = VK_FORMAT_MAX_ENUM;
    //     for ( const auto &img : _swapchainImages )
    //     {
    //         vkDestroySemaphore( tools::getDevice(), img.isAvailable, ALLOCATION_CALLBACK );
    //         vkDestroySemaphore( tools::getDevice(), img.isRendered, ALLOCATION_CALLBACK );
    //         delete getImage( img.image );
    //     }
    //     vkDestroySwapchainKHR( tools::getDevice(), _swapchain, ALLOCATION_CALLBACK );
    // }

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