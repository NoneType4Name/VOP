#include <swapchain.hxx>
#include <device.hxx>
#include <surface.hxx>
#include <instance.hxx>
#include <common/logging.hxx>

namespace Engine
{
    swapchain::swapchain( bool, types::device device, window::types::window window )
    {
        DEFINE_DATA_FIELD( device, window );
        data->device->data->swapchains.emplace( this );
        data->window->data->swapchains.emplace( this );
        uint32_t c;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR( device->data->description->data->phDevice, window->surface, &properties.capabilities );
        glfwSetWindowSizeLimits( window->glfwHandle, properties.capabilities.minImageExtent.width, properties.capabilities.minImageExtent.height, properties.capabilities.maxImageExtent.width, properties.capabilities.maxImageExtent.height );
        vkGetPhysicalDeviceSurfaceFormatsKHR( device->data->description->data->phDevice, window->surface, &c, nullptr );
        properties.formats.resize( c );
        vkGetPhysicalDeviceSurfaceFormatsKHR( device->data->description->data->phDevice, window->surface, &c, properties.formats.data() );
        vkGetPhysicalDeviceSurfacePresentModesKHR( device->data->description->data->phDevice, window->surface, &c, nullptr );
        properties.presentModes.resize( c );
        vkGetPhysicalDeviceSurfacePresentModesKHR( device->data->description->data->phDevice, window->surface, &c, properties.presentModes.data() );
    }

    swapchain::swapchain( types::device device, window::types::window window ) :
        swapchain( 1, device, window )
    {
        VkSwapchainCreateInfoKHR createInfo {};
        VkSurfaceFormatKHR SurfaceFormat { properties.formats[ 0 ] };
        for ( const auto &format : properties.formats )
        {
            VkFormatProperties properties;
            vkGetPhysicalDeviceFormatProperties( device->data->description->data->phDevice, format.format, &properties );
            if ( format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR ) SurfaceFormat = format;
        }

        presentMode = VK_PRESENT_MODE_FIFO_KHR;
        for ( const auto &mode : properties.presentModes )
        {
            if ( mode == VK_PRESENT_MODE_MAILBOX_KHR ) presentMode = mode;
        }

        int width, height;
        glfwGetFramebufferSize( data->window->glfwHandle, &width, &height );
        createInfo.imageUsage         = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        createInfo.imageFormat        = SurfaceFormat.format;
        createInfo.imageExtent.width  = std::clamp( static_cast<uint32_t>( width ), properties.capabilities.minImageExtent.width, properties.capabilities.maxImageExtent.width );
        createInfo.imageExtent.height = std::clamp( static_cast<uint32_t>( height ), properties.capabilities.minImageExtent.height, properties.capabilities.maxImageExtent.height );
        createInfo.minImageCount      = properties.capabilities.maxImageCount;
        createInfo.clipped            = VK_FALSE;
        createInfo.oldSwapchain       = handle;
        createInfo.preTransform       = properties.capabilities.currentTransform;
        createInfo.compositeAlpha     = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        data->create( createInfo );
        // DEFINE_DATA_FIELD( device, window );
        // // data->device->data->regSwapchain( this );
        // data->device->data->swapchains.emplace( this );
        // data->window->data->swapchains.emplace( this );
        // std::vector<void *> swapchainData;
        // std::vector<void *> pData;
        // data->window->data->instance->data->setup->swapchainInfo( this, data->createInfo, swapchainData, data->window->data->instance->data->userPointer );
        // CHECK_RESULT( vkCreateSwapchainKHR( data->device->handle, &data->createInfo, ENGINE_ALLOCATION_CALLBACK, &data->swapchain ) );
        // data->setupImgs();
    }

    swapchain::~swapchain()
    {
        auto img { images.begin() };
        while ( img != images.end() )
            delete *img++;
        vkDestroySwapchainKHR( data->device->handle, handle, ENGINE_ALLOCATION_CALLBACK );
        data->device->data->swapchains.erase( this );
        data->window->data->swapchains.erase( this );
    };

    void swapchain::DATA_TYPE::create( VkSwapchainCreateInfoKHR createInfo )
    {
        createInfo.sType   = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = window->surface;
        if ( createInfo.minImageCount < parent->properties.capabilities.minImageCount )
            createInfo.minImageCount = parent->properties.capabilities.minImageCount;
        parent->format.format                         = createInfo.imageFormat;
        parent->format.colorSpace                     = createInfo.imageColorSpace;
        parent->presentMode                           = createInfo.presentMode;
        createInfo.imageExtent.width                  = std::clamp( static_cast<uint32_t>( createInfo.imageExtent.width ), parent->properties.capabilities.minImageExtent.width, parent->properties.capabilities.maxImageExtent.width );
        createInfo.imageExtent.height                 = std::clamp( static_cast<uint32_t>( createInfo.imageExtent.height ), parent->properties.capabilities.minImageExtent.height, parent->properties.capabilities.maxImageExtent.height );
        parent->properties.capabilities.currentExtent = createInfo.imageExtent;
        if ( !createInfo.imageArrayLayers )
            createInfo.imageArrayLayers = 1;
        if ( !createInfo.queueFamilyIndexCount )
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        else
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.oldSwapchain = parent->handle;
        vkCreateSwapchainKHR( device->handle, &createInfo, ENGINE_ALLOCATION_CALLBACK, &parent->handle );
        uint32_t c;
        vkGetSwapchainImagesKHR( device->handle, parent->handle, &c, nullptr );
        std::vector<VkImage> imgs { c };
        parent->images.resize( c );
        vkGetSwapchainImagesKHR( device->handle, parent->handle, &c, imgs.data() );
        for ( size_t i { 0 }; i < imgs.size(); i++ )
        {
            VkImageCreateInfo imCreateInfo {};
            imCreateInfo.imageType     = VK_IMAGE_TYPE_2D;
            imCreateInfo.arrayLayers   = createInfo.imageArrayLayers;
            imCreateInfo.samples       = VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM;
            imCreateInfo.usage         = createInfo.imageUsage;
            imCreateInfo.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            imCreateInfo.extent        = { createInfo.imageExtent.width, createInfo.imageExtent.height, 1 };
            parent->images[ i ]        = new image { device, imCreateInfo, { .image = imgs[ i ], .viewType = VK_IMAGE_VIEW_TYPE_2D, .format = parent->format.format, .subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 } }, 0 };
        }
    }

    swapchain::DATA_TYPE::DATA_TYPE( types::swapchain parent, types::device device, window::types::window window ) :
        parent { parent }
    {
        this->device = device;
        this->window = window;
    }

    swapchain::DATA_TYPE::~DATA_TYPE()
    {
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