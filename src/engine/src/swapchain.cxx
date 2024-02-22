#include <swapchain.hxx>
#include <device.hxx>
#include <surface.hxx>
#include <instance.hxx>
#include <common/logging.hxx>

namespace Engine
{
    swapchain::swapchain( types::device device, types::surface surface )
    {
        OBJECTIVE_VULKAN_OBJECTIVE_VULKAN_DEFINE_DATA_FIELD( device, surface );
        data->device->data->swapchains.emplace( this );
        data->surface->data->swapchains.emplace( this );
        uint32_t c;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR( device->data->description->data->phDevice, surface->handle, &properties.capabilities );
        vkGetPhysicalDeviceSurfaceFormatsKHR( device->data->description->data->phDevice, surface->handle, &c, nullptr );
        properties.formats.resize( c );
        vkGetPhysicalDeviceSurfaceFormatsKHR( device->data->description->data->phDevice, surface->handle, &c, properties.formats.data() );
        vkGetPhysicalDeviceSurfacePresentModesKHR( device->data->description->data->phDevice, surface->handle, &c, nullptr );
        properties.presentModes.resize( c );
        vkGetPhysicalDeviceSurfacePresentModesKHR( device->data->description->data->phDevice, surface->handle, &c, properties.presentModes.data() );
    }

    swapchain::~swapchain()
    {
        auto img { images.begin() };
        while ( img != images.end() )
            delete *img++;
        vkDestroySwapchainKHR( data->device->handle, handle, OBJECTIVE_VULKAN_ALLOCATION_CALLBACK );
        data->device->data->swapchains.erase( this );
        data->surface->data->swapchains.erase( this );
    };

    void swapchain::OBJECTIVE_VULKAN_DATA_TYPE::create( VkSwapchainCreateInfoKHR createInfo )
    {
        createInfo.sType   = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = surface->handle;
        if ( createInfo.minImageCount < parent->properties.capabilities.minImageCount )
            createInfo.minImageCount = parent->properties.capabilities.minImageCount;
        parent->format.format                         = createInfo.imageFormat;
        parent->format.colorSpace                     = createInfo.imageColorSpace;
        parent->presentMode                           = createInfo.presentMode;
        createInfo.imageExtent.width                  = std::clamp( static_cast<uint32_t>( parent->properties.capabilities.currentExtent.width ), parent->properties.capabilities.minImageExtent.width, parent->properties.capabilities.maxImageExtent.width );
        createInfo.imageExtent.height                 = std::clamp( static_cast<uint32_t>( parent->properties.capabilities.currentExtent.height ), parent->properties.capabilities.minImageExtent.height, parent->properties.capabilities.maxImageExtent.height );
        parent->properties.capabilities.currentExtent = createInfo.imageExtent;
        if ( !createInfo.imageArrayLayers )
            createInfo.imageArrayLayers = 1;
        if ( !createInfo.queueFamilyIndexCount )
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        else
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.oldSwapchain = parent->handle;
        OBJECTIVE_VULKAN_CHECK_RESULT( vkCreateSwapchainKHR( device->handle, &createInfo, OBJECTIVE_VULKAN_ALLOCATION_CALLBACK, &parent->handle ) );
        if ( parent->handle )
            vkDestroySwapchainKHR( device->handle, createInfo.oldSwapchain, OBJECTIVE_VULKAN_ALLOCATION_CALLBACK );
        if ( createInfo.oldSwapchain )
            for ( const auto &img : parent->images )
                delete img;
        uint32_t c;
        OBJECTIVE_VULKAN_CHECK_RESULT( vkGetSwapchainImagesKHR( device->handle, parent->handle, &c, nullptr ) );
        std::vector<VkImage> imgs { c };
        parent->images.resize( c );
        OBJECTIVE_VULKAN_CHECK_RESULT( vkGetSwapchainImagesKHR( device->handle, parent->handle, &c, imgs.data() ) );
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

    void swapchain::reCreate()
    {
        VkSwapchainCreateInfoKHR createInfo {};
        presentMode = VK_PRESENT_MODE_FIFO_KHR;
        for ( const auto &mode : properties.presentModes )
        {
            if ( mode == VK_PRESENT_MODE_MAILBOX_KHR ) presentMode = mode;
        }

        createInfo.imageUsage         = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        createInfo.imageFormat        = format.format;
        createInfo.imageColorSpace    = format.colorSpace;
        createInfo.imageExtent.width  = data->surface->data->width;
        createInfo.imageExtent.height = data->surface->data->height;
        createInfo.minImageCount      = properties.capabilities.minImageCount;
        createInfo.clipped            = VK_FALSE;
        createInfo.oldSwapchain       = handle;
        createInfo.preTransform       = properties.capabilities.currentTransform;
        createInfo.presentMode        = presentMode;
        createInfo.compositeAlpha     = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        data->create( createInfo );
    }

    swapchain::OBJECTIVE_VULKAN_DATA_TYPE::OBJECTIVE_VULKAN_DATA_TYPE( types::swapchain parent, types::device device, types::surface surface ) :
        parent { parent }
    {
        this->device  = device;
        this->surface = surface;
    }

    swapchain::OBJECTIVE_VULKAN_DATA_TYPE::~OBJECTIVE_VULKAN_DATA_TYPE()
    {
    }
} // namespace Engine