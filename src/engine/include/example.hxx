#ifndef VK_USE_PLATFORM_WIN32_KHR
#    define VK_USE_PLATFORM_WIN32_KHR
#endif
#include <ObjectiveVulkan.hxx>
#include <ehi.hxx>

namespace Engine
{
    namespace examples
    {
        class instance : public Engine::instance
        {
          public:
            instance( const char *appName, uint32_t appVersion ) :
                Engine::instance( appName, appVersion )
            {
                VkApplicationInfo ApplicationInfo {};
                ApplicationInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
                ApplicationInfo.apiVersion         = VK_API_VERSION_1_0;
                ApplicationInfo.pApplicationName   = appName;
                ApplicationInfo.applicationVersion = appVersion;

                VkDebugUtilsMessengerCreateInfoEXT debugUtilsMsgCI { data->setupDebugLayerCallback() };
                std::vector<VkValidationFeatureEnableEXT> enabledVFeatures { VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT };
                VkValidationFeaturesEXT ValidationFeatures {};
                ValidationFeatures.sType                          = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
                ValidationFeatures.enabledValidationFeatureCount  = enabledVFeatures.size();
                ValidationFeatures.pEnabledValidationFeatures     = enabledVFeatures.data();
                ValidationFeatures.disabledValidationFeatureCount = 0;
                ValidationFeatures.pDisabledValidationFeatures    = nullptr;
                debugUtilsMsgCI.pNext                             = &ValidationFeatures;

                std::vector<const char *> ext { VK_KHR_WIN32_SURFACE_EXTENSION_NAME };
                std::vector<const char *> lays {};

                VkInstanceCreateInfo InstanceCreateInfo {};
                InstanceCreateInfo.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
                InstanceCreateInfo.pNext                   = &debugUtilsMsgCI;
                InstanceCreateInfo.enabledLayerCount       = lays.size();
                InstanceCreateInfo.ppEnabledLayerNames     = lays.data();
                InstanceCreateInfo.enabledExtensionCount   = ext.size();
                InstanceCreateInfo.ppEnabledExtensionNames = ext.data();
                InstanceCreateInfo.pApplicationInfo        = &ApplicationInfo;
                data->create( InstanceCreateInfo );
                debugUtilsMsgCI.pNext = nullptr;
                data->initDebugLayerCallBack( debugUtilsMsgCI );
            }
        };

        class surface : public Engine::surface
        {
          public:
            surface( Engine::instance *instance, uint32_t w, uint32_t h, HWND hwnd ) :
                Engine::surface( instance, w, h )
            {
                VkWin32SurfaceCreateInfoKHR createInfo {};
                createInfo.sType     = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
                createInfo.hinstance = GetModuleHandle( nullptr );
                createInfo.hwnd      = hwnd;
                vkCreateWin32SurfaceKHR( instance->handle, &createInfo, OBJECTIVE_VULKAN_ALLOCATION_CALLBACK, &handle );
            }
        };

        class swapchain : public Engine::swapchain
        {
          public:
            swapchain( types::device device, types::surface surface ) :
                Engine::swapchain( device, surface )

            {
                VkSwapchainCreateInfoKHR createInfo {};
                VkSurfaceFormatKHR SurfaceFormat { properties.formats[ 0 ] };
                for ( const auto &format : properties.formats )
                {
                    VkFormatProperties properties;
                    vkGetPhysicalDeviceFormatProperties( data->device->data->description->data->phDevice, format.format, &properties );
                    if ( format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR ) SurfaceFormat = format;
                }

                presentMode = VK_PRESENT_MODE_FIFO_KHR;
                for ( const auto &mode : properties.presentModes )
                {
                    if ( mode == VK_PRESENT_MODE_MAILBOX_KHR ) presentMode = mode;
                }

                createInfo.imageUsage         = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
                createInfo.imageFormat        = SurfaceFormat.format;
                createInfo.imageColorSpace    = SurfaceFormat.colorSpace;
                createInfo.imageExtent.width  = std::clamp( properties.capabilities.currentExtent.width, properties.capabilities.minImageExtent.width, properties.capabilities.maxImageExtent.width );
                createInfo.imageExtent.height = std::clamp( properties.capabilities.currentExtent.width, properties.capabilities.minImageExtent.height, properties.capabilities.maxImageExtent.height );
                createInfo.minImageCount      = properties.capabilities.minImageCount;
                createInfo.clipped            = VK_FALSE;
                createInfo.oldSwapchain       = handle;
                createInfo.preTransform       = properties.capabilities.currentTransform;
                createInfo.presentMode        = presentMode;
                createInfo.compositeAlpha     = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
                data->create( createInfo );
            }
        };

        class device : public Engine::device
        {
          public:
            device( types::deviceDescription description, std::vector<types::surface> surfaces ) :
                Engine::device( description, surfaces )
            {
                VkDeviceCreateInfo DeviceCreateInfo {};
                VkPhysicalDeviceFeatures features {};
                features.samplerAnisotropy = VK_TRUE;
                features.sampleRateShading = VK_TRUE;
                std::vector<VkDeviceQueueCreateInfo> queues;
                std::vector<float> priorities;
                uint32_t i { 0 };
                for ( uint32_t i { 0 }; i < data->description->data->queueFamilyProperties.size(); ++i )
                {
                    VkBool32 present;
                    for ( const auto &srf : surfaces )
                    {
                        vkGetPhysicalDeviceSurfaceSupportKHR( data->description->data->phDevice, i, srf->handle, &present );
                        if ( !present )
                            break;
                    }
                    if ( data->description->data->queueFamilyProperties[ i ].queueFlags & ( VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_TRANSFER_BIT ) && present )
                    {
                        priorities.emplace_back( 1.f );
                        queues.emplace_back( VkDeviceQueueCreateInfo { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, nullptr, 0, i, 1, priorities.data() } );
                        break;
                    }
                }
                DeviceCreateInfo.queueCreateInfoCount = queues.size();
                DeviceCreateInfo.pQueueCreateInfos    = queues.data();
                VkPhysicalDeviceFeatures2 physicalDeviceFeatures {};
                DeviceCreateInfo.pEnabledFeatures = &features;
                data->create( DeviceCreateInfo );
                universalQueue = getQueue( 0, 0 );
                for ( auto &wnd : surfaces )
                {
                    new Engine::examples::swapchain { this, wnd };
                }
            }
        };

        class renderPass : public Engine::renderPass
        {
          public:
            renderPass( types::device device, VkRenderPassCreateInfo createInfo ) :
                Engine::renderPass( device, createInfo )
            {
                data->create( createInfo );
            }
        };

        class framebuffer : public Engine::framebuffer
        {
          public:
            framebuffer( types::renderPass renderPass, std::vector<types::image> attachments ) :
                Engine::framebuffer( renderPass, attachments )
            {
                uint32_t w { ~0u }, h { ~0u }, l { ~0u };
                for ( const auto &atch : attachments )
                {
                    if ( atch->properties.extent.width < w )
                        w = atch->properties.extent.width;
                    if ( atch->properties.extent.height < h )
                        h = atch->properties.extent.height;
                    if ( atch->properties.extent.depth < l )
                        l = atch->properties.extent.depth;
                }
                VkFramebufferCreateInfo createInfo { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
                createInfo.width  = w;
                createInfo.height = h;
                createInfo.layers = l;
                data->create( createInfo, attachments );
            }
        };
    } // namespace examples
} // namespace Engine