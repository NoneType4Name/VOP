#include <engine.hxx>
#include <spdlog/spdlog.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <glm/gtx/hash.hpp>
#include <tiny_obj_loader.h>
#include <glm/gtc/matrix_transform.hpp>
#include <vulkan/vk_enum_string_helper.h>
#include <format>
#include <iostream>
#define ENGINE_VERSION VK_MAKE_VERSION( 0, 0, 1 )

namespace Engine
{

    namespace
    {
        const std::vector<const char *> ValidationLayers{ "VK_LAYER_KHRONOS_validation" };
        const std::vector<const char *> RequeredDeviceExts{ VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_SHADER_NON_SEMANTIC_INFO_EXTENSION_NAME };

        struct Vertex
        {
            glm::vec3 coordinate;
            glm::vec4 color;
            glm::vec2 texture;
            static VkVertexInputBindingDescription InputBindingDescription()
            {
                VkVertexInputBindingDescription VertexInputBindingDescription{};
                VertexInputBindingDescription.binding   = 0;
                VertexInputBindingDescription.stride    = sizeof( Vertex );
                VertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
                return VertexInputBindingDescription;
            }

            static std::array<VkVertexInputAttributeDescription, 3> InputAttributeDescription()
            {
                std::array<VkVertexInputAttributeDescription, 3> VertexInputAttributeDescription{};
                VertexInputAttributeDescription[ 0 ].binding  = 0;
                VertexInputAttributeDescription[ 0 ].location = 0;
                VertexInputAttributeDescription[ 0 ].format   = VK_FORMAT_R32G32B32_SFLOAT;
                VertexInputAttributeDescription[ 0 ].offset   = offsetof( Vertex, coordinate );

                VertexInputAttributeDescription[ 1 ].binding  = 0;
                VertexInputAttributeDescription[ 1 ].location = 1;
                VertexInputAttributeDescription[ 1 ].format   = VK_FORMAT_R32G32B32A32_SFLOAT;
                VertexInputAttributeDescription[ 1 ].offset   = offsetof( Vertex, color );

                VertexInputAttributeDescription[ 2 ].binding  = 0;
                VertexInputAttributeDescription[ 2 ].location = 2;
                VertexInputAttributeDescription[ 2 ].format   = VK_FORMAT_R32G32_SFLOAT;
                VertexInputAttributeDescription[ 2 ].offset   = offsetof( Vertex, texture );
                return VertexInputAttributeDescription;
            }
            bool operator==( const Vertex &other ) const
            {
                return coordinate == other.coordinate && color == other.color && texture == other.texture;
            }
        };

        struct Model
        {
            std::vector<Vertex> ModelVertecies;
            std::vector<uint32_t> ModelVerteciesIndices;
            uint32_t VerteciesOffset{};
            uint32_t IndeciesOffset{};
        };

        struct DemensionsUniformrObject
        {
            alignas( 16 ) glm::mat4 model;
            alignas( 16 ) glm::mat4 view;
            alignas( 16 ) glm::mat4 proj;
        };

        struct QueueFamilyIndices
        {
            std::optional<uint32_t> graphic;
            std::optional<uint32_t> present;
            std::optional<uint32_t> transfer;
            // std::optional<uint32_t> compute;

            bool isComplete()
            {
                return graphic.has_value() && present.has_value() && transfer.has_value(); // && compute.has_value()
            }
        };

        struct SwapChain
        {
            VkSwapchainKHR Swapchain;
            VkFormat Format;
            VkPresentModeKHR PresentMode;
            VkSurfaceCapabilitiesKHR Capabilities;
            std::vector<VkSurfaceFormatKHR> AviliableFormats;
            std::vector<VkPresentModeKHR> AviliablePresentModes;
        };

        struct _physicalDevice
        {
            VkPhysicalDevice device;
            VkPhysicalDeviceProperties properties;
            VkPhysicalDeviceFeatures features;
            SwapChain swapchain;
            QueueFamilyIndices Indecies;
        };

        struct _logicalDevice
        {
            _physicalDevice *physicalDevice;
            VkDevice device;
            VkPhysicalDeviceFeatures enabledFeatures{};
            VkQueue graphicQueue;
            VkQueue presentQueue;
            VkQueue transferQueue;
            // VkQueue computeQueue;
            _logicalDevice( _physicalDevice &pDevice )
            {
                physicalDevice = &pDevice;
                std::vector<VkDeviceQueueCreateInfo> QueuesCreateInfo{};
                std::unordered_map<uint32_t, float> QueueFamiliesPriority{
                    { physicalDevice->Indecies.graphic.value(), 1.0f },
                    { physicalDevice->Indecies.present.value(), 1.0f },
                    { physicalDevice->Indecies.transfer.value(), 1.0f },
                    // { _selectedPhysicalDevice->Indecies.compute.value(), 1.0f },
                };
                for( auto &QueueFamily : QueueFamiliesPriority )
                {
                    VkDeviceQueueCreateInfo QueueCreateInfo{};
                    QueueCreateInfo.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                    QueueCreateInfo.queueFamilyIndex = QueueFamily.first;
                    QueueCreateInfo.queueCount       = 1;
                    QueueCreateInfo.pQueuePriorities = &QueueFamily.second;
                    QueuesCreateInfo.push_back( QueueCreateInfo );
                }
                enabledFeatures.samplerAnisotropy = VK_TRUE;
                enabledFeatures.sampleRateShading = VK_TRUE;
                VkDeviceCreateInfo createInfo{};
                createInfo.sType                = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
                createInfo.queueCreateInfoCount = static_cast<uint32_t>( QueuesCreateInfo.size() );
                createInfo.pQueueCreateInfos    = QueuesCreateInfo.data();
                createInfo.pEnabledFeatures     = &enabledFeatures;
                // createInfo.pEnabledFeatures        = &PhysicalDeviceFeatures;
                createInfo.enabledExtensionCount   = static_cast<uint32_t>( RequeredDeviceExts.size() );
                createInfo.ppEnabledExtensionNames = RequeredDeviceExts.data();
#ifdef ENGINE_DEBUG // to do
                createInfo.enabledLayerCount   = static_cast<uint32_t>( ValidationLayers.size() );
                createInfo.ppEnabledLayerNames = ValidationLayers.data();
#else
                createInfo.enabledLayerCount = 0;
#endif
                VkResult Result = vkCreateDevice( physicalDevice->device, &createInfo, nullptr, &device );
                if( Result != VK_SUCCESS )
                    SPDLOG_CRITICAL( "Failed to Create logical Device, error: {}", string_VkResult( Result ) );
            }

            ~_logicalDevice()
            {
                vkDestroyDevice( device, nullptr );
            }
        };

        uint16_t _width{ 800ui16 };
        uint16_t _height{ 600ui16 };
        uint16_t DisplayWidth;
        uint16_t DisplayHeight;
        std::string _title;
        GLFWwindow *_window{ nullptr };
        GLFWmonitor *_monitor{ nullptr };
        VkInstance _instance;
        VkSurfaceKHR _surface;
        _logicalDevice *_selectedLogicalDevice;
        std::vector<_physicalDevice> _avilablePhysicalDevices;
        KeyEventCallBack _KeyEventCallBack{ nullptr };

        struct _
        {
            _()
            {
                spdlog::set_level( DEBUG ? spdlog::level::trace : spdlog::level::critical );
                spdlog::set_pattern( "[%H:%M:%S.%e] [%^%l%$] %v" );
                if( glfwInit() )
                {
                    SPDLOG_DEBUG( "GLFW{} inititialized.", glfwGetVersionString() );
                    glfwSetErrorCallback( []( int code, const char *data )
                                          { SPDLOG_ERROR( "GLFW ERROR {}: {}", code, data ); } );
                }
                else
                {
                    SPDLOG_CRITICAL( "GLFW not initialized." );
                    SPDLOG_INFO( "Exit with code {}.", EXIT_FAILURE );
                    exit( EXIT_FAILURE );
                }
            }
            ~_()
            {
                vkDestroySurfaceKHR( _instance, _surface, nullptr );
                glfwTerminate();
            }
        } _;

        void _getScreenResolution( uint16_t &width, uint16_t &height )
        {
            if( _monitor == nullptr )
                _monitor = glfwGetPrimaryMonitor();
            else
                _monitor = glfwGetWindowMonitor( _window );
            glfwGetVideoMode( _monitor );
            auto mode = glfwGetVideoMode( _monitor );
            width     = mode->width;
            height    = mode->height;
        }

        inline GrapchicPhysicalDevice _getPhysicalDeviceStruct( _physicalDevice *_device )
        {
            return { _device->properties.deviceName, GrapchiDeviceType( _device->properties.deviceType ), _device->properties.deviceID };
        };

        QueueFamilyIndices _physicalDeviceGetIndecies( VkPhysicalDevice _dev )
        {
            QueueFamilyIndices _indices;
            uint32_t _c{ 0 };
            vkGetPhysicalDeviceQueueFamilyProperties( _dev, &_c, nullptr );
            std::vector<VkQueueFamilyProperties> QueueFamilies( _c );
            vkGetPhysicalDeviceQueueFamilyProperties( _dev, &_c, QueueFamilies.data() );
            _c = 0;
            for( const auto &queueF : QueueFamilies )
            {
                VkBool32 presentSupport{ false };
                vkGetPhysicalDeviceSurfaceSupportKHR( _dev, _c, _surface, &presentSupport );
                if( !_indices.graphic.has_value() && queueF.queueFlags & VK_QUEUE_GRAPHICS_BIT )
                {
                    _indices.graphic = _c;
                }
                if( !_indices.present.has_value() && presentSupport ) _indices.present = _c;
                else if( !_indices.transfer.has_value() && queueF.queueFlags & VK_QUEUE_TRANSFER_BIT )
                    _indices.transfer = _c;
                // else if( !_indices.compute.has_value() && queueF.queueFlags & VK_QUEUE_COMPUTE_BIT )
                //     _indices.compute = index;
                else
                    break;
                _c++;
            }
            return _indices;
        }

        void FramebufferResizeCallback( GLFWwindow *window, int w, int h )
        {
            SetWindowResolution( w, h );
            // TO DO: update frame buffer size (recreate swapchain).
        }

        inline void WindwoResizeCallback( GLFWwindow *window, int w, int h )
        {
            FramebufferResizeCallback( window, w, h );
        }

        void _KeyCallbackManager( GLFWwindow *wnd, int key, int scancode, int action, int mods )
        {
            _KeyEventCallBack( key, scancode, action, mods );
        }

        static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT MessageLevel,
            VkDebugUtilsMessageTypeFlagsEXT MessageType,
            const VkDebugUtilsMessengerCallbackDataEXT *CallbackData,
            void *UserData )
        {
            // VulkanInstance *Vulkan = static_cast<VulkanInstance *>( UserData ); // Not used.
            std::string StrMessageType;
            switch( MessageType )
            {
                case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
                    StrMessageType = "GeneralError";
                    break;
                case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
                    StrMessageType = "SpecificationError";
                    break;
                case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
                    StrMessageType = "MisuseVulkanApiError";
                    break;

                default:
                    StrMessageType = string_VkDebugUtilsMessageTypeFlagsEXT( MessageType );
                    break;
            }
            switch( static_cast<uint32_t>( MessageLevel ) )
            {
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
                    SPDLOG_DEBUG( std::format( "{} message: {}", ( MessageType == VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT ? "" : format( "Type: {}, ", StrMessageType ) ), CallbackData->pMessage ) );
                    break;
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                    SPDLOG_INFO( std::format( "{} message: {}", ( MessageType == VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT ? "" : format( "Type: {}, ", StrMessageType ) ), CallbackData->pMessage ) );
                    break;
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                    SPDLOG_WARN( std::format( "{} message: {}", ( MessageType == VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT ? "" : format( "Type: {}, ", StrMessageType ) ), CallbackData->pMessage ) );
                    break;
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                    SPDLOG_CRITICAL( std::format( "{} message: {}", ( MessageType == VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT ? "" : format( "Type: {}, ", StrMessageType ) ), CallbackData->pMessage ) );
                    return VK_TRUE;
                    break;
            }
            return VK_FALSE;
        }

    } // namespace

    void WindowInit( uint16_t width, uint16_t height, const char *title )
    {
        glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );
        glfwWindowHint( GLFW_RESIZABLE, GLFW_TRUE );
        _window = glfwCreateWindow( width ? width : _width, height ? height : _height, title, nullptr, nullptr );
        _title  = title;
        SetWindowResolution( width, height );
        glfwSetFramebufferSizeCallback( _window, FramebufferResizeCallback );
        glfwSetWindowSizeCallback( _window, WindwoResizeCallback );
        CentralizeWindow();

        VkInstanceCreateInfo InstanceCreateInfo{};
        InstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        uint32_t glfwExtensionsCount{ 0 };
        const char **glfwExtensions = glfwGetRequiredInstanceExtensions( &glfwExtensionsCount );
        std::vector<const char *> glfwExtensionsVector( glfwExtensions, glfwExtensions + glfwExtensionsCount );

#ifdef _DEBUG
        uint32_t _c;
        vkEnumerateInstanceLayerProperties( &_c, nullptr );
        // VkLayerProperties *AviableLayers = new VkLayerProperties[ _c ];
        std::vector<VkLayerProperties> AviableLayers{ _c };
        vkEnumerateInstanceLayerProperties( &_c, AviableLayers.data() );
        size_t c{ ValidationLayers.size() };
        std::vector<const char *> NotAvilableLayers{ ValidationLayers.begin(), ValidationLayers.end() };
        for( size_t i{ 0 }; i < c; i++ )
        {
            for( uint32_t _i{ 0 }; _i < _c; _i++ )
                if( !strcmp( NotAvilableLayers[ c - 1 - i ], AviableLayers[ _i ].layerName ) )
                {
                    NotAvilableLayers.erase( NotAvilableLayers.end() - i - 1 );
                    break;
                }
            if( NotAvilableLayers.empty() )
                break;
        }
        if( !NotAvilableLayers.empty() )
        {
            std::string Err = std::format( "Not finded validation layers: {}:\n", std::to_string( NotAvilableLayers.size() ) );
            for( const auto VL : NotAvilableLayers )
            {
                Err += std::format( "\t {}\n", VL );
            }
            SPDLOG_CRITICAL( Err );
        }

        VkValidationFeatureEnableEXT enabled[]{ VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT };
        VkValidationFeaturesEXT ValidationFeatures{};
        ValidationFeatures.sType                          = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
        ValidationFeatures.enabledValidationFeatureCount  = sizeof( enabled ) / sizeof( enabled[ 0 ] );
        ValidationFeatures.pEnabledValidationFeatures     = enabled;
        ValidationFeatures.disabledValidationFeatureCount = 0;
        ValidationFeatures.pDisabledValidationFeatures    = nullptr;

        VkDebugUtilsMessengerCreateInfoEXT DebugUtilsMessengerCreateInfoEXT{};
        InstanceCreateInfo.pNext                         = &DebugUtilsMessengerCreateInfoEXT;
        DebugUtilsMessengerCreateInfoEXT.sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        DebugUtilsMessengerCreateInfoEXT.pNext           = &ValidationFeatures;
        DebugUtilsMessengerCreateInfoEXT.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        DebugUtilsMessengerCreateInfoEXT.messageType     = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        DebugUtilsMessengerCreateInfoEXT.pfnUserCallback = DebugCallback;
        // DebugUtilsMessengerCreateInfoEXT.pUserData       = this; // No used.
        glfwExtensionsVector.push_back( VK_EXT_DEBUG_UTILS_EXTENSION_NAME );
        InstanceCreateInfo.enabledLayerCount   = ValidationLayers.size();
        InstanceCreateInfo.ppEnabledLayerNames = ValidationLayers.data();

#endif
        InstanceCreateInfo.enabledExtensionCount   = glfwExtensionsVector.size();
        InstanceCreateInfo.ppEnabledExtensionNames = glfwExtensionsVector.data();
        VkApplicationInfo ApplicationInfo{};
        InstanceCreateInfo.pApplicationInfo = &ApplicationInfo;
        ApplicationInfo.sType               = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        ApplicationInfo.engineVersion       = 0;
        ApplicationInfo.apiVersion          = VK_API_VERSION_1_0;
        auto app_name{ "Grapchic engine of " + _title };
        ApplicationInfo.pApplicationName   = app_name.c_str();
        ApplicationInfo.applicationVersion = ENGINE_VERSION;
        VkResult Result{ vkCreateInstance( &InstanceCreateInfo, nullptr, &_instance ) };

#if defined( _WIN32 )
        VkWin32SurfaceCreateInfoKHR Win32SurfaceCreateInfo{
            VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
            nullptr,
            0,
            GetModuleHandle( nullptr ),
            glfwGetWin32Window( _window ) };
        Result = vkCreateWin32SurfaceKHR( _instance, &Win32SurfaceCreateInfo, nullptr, &_surface );
// #if defined( __LINUX__ )

//         VkXlibSurfaceCreateInfoKHR XlibSurfaceCreateInfoKHR{
//             VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR,
//             nullptr,
//             0,
//             &dpy,
//             &window };
//         Result = vkCreateXlibSurfaceKHR( Instance, &XlibSurfaceCreateInfoKHR, nullptr, &Screen )
#endif

        if( Result != VK_SUCCESS )
        {
            SPDLOG_CRITICAL( "Failed to Create surface, error" );
        }
        vkEnumeratePhysicalDevices( _instance, &_c, nullptr );
        std::vector<VkPhysicalDevice> _avilableDevices( _c );
        _avilablePhysicalDevices.reserve( _c );
        _avilablePhysicalDevices.clear();
        vkEnumeratePhysicalDevices( _instance, &_c, _avilableDevices.data() );
        for( auto device : _avilableDevices )
        {
            uint16_t mark{ 0 };
            SwapChain swpchnprprts;
            VkPhysicalDeviceProperties PhysicalDeviceProperties{};
            VkPhysicalDeviceFeatures PhysicalDeviceFeatures{};
            vkGetPhysicalDeviceProperties( device, &PhysicalDeviceProperties );
            vkGetPhysicalDeviceFeatures( device, &PhysicalDeviceFeatures );

            // if( PhysicalDeviceProperties.deviceType )
            //     mark = 5 - PhysicalDeviceProperties.deviceType;

            vkGetPhysicalDeviceSurfaceCapabilitiesKHR( device, _surface, &swpchnprprts.Capabilities );
            uint32_t formatsCount{ 0 };
            vkGetPhysicalDeviceSurfaceFormatsKHR( device, _surface, &formatsCount, nullptr );
            swpchnprprts.AviliableFormats.resize( formatsCount );
            vkGetPhysicalDeviceSurfaceFormatsKHR( device, _surface, &formatsCount, swpchnprprts.AviliableFormats.data() );
            uint32_t PresentModesCount{ 0 };
            vkGetPhysicalDeviceSurfacePresentModesKHR( device, _surface, &PresentModesCount, nullptr );
            swpchnprprts.AviliablePresentModes.resize( PresentModesCount );
            vkGetPhysicalDeviceSurfacePresentModesKHR( device, _surface, &PresentModesCount, swpchnprprts.AviliablePresentModes.data() );

            _avilablePhysicalDevices.push_back( {
                device,
                PhysicalDeviceProperties,
                PhysicalDeviceFeatures,
                swpchnprprts,
                _physicalDeviceGetIndecies( device ),
            } );
        }
        std::cout << _avilablePhysicalDevices[ 0 ].properties.deviceName << std::endl;
        _selectedLogicalDevice = new _logicalDevice( _avilablePhysicalDevices[ 0 ] );
    }

    void SetGraphicDevice( GrapchicPhysicalDevice device )
    {
        for( auto dev : _avilablePhysicalDevices )
        {
            if( dev.properties.deviceID == device.deviceID )
            {
                delete _selectedLogicalDevice;
                _selectedLogicalDevice = new _logicalDevice( dev );
                return;
            }
        }
    }

    inline GrapchicPhysicalDevice GetActiveGrapchiDevice()
    {
        return { _selectedLogicalDevice->physicalDevice->properties.deviceName, GrapchiDeviceType( _selectedLogicalDevice->physicalDevice->properties.deviceType ), _selectedLogicalDevice->physicalDevice->properties.deviceID };
    }

    std::vector<GrapchicPhysicalDevice> GetGraphicDevices()
    {
        std::vector<GrapchicPhysicalDevice> _devices;
        for( const auto &_dev : _avilablePhysicalDevices )
        {
            _devices.push_back( { _dev.properties.deviceName,
                                  GrapchiDeviceType( _dev.properties.deviceType ),
                                  _dev.properties.deviceID,
                                  { _dev.properties.limits.framebufferDepthSampleCounts } } );
        }
        return _devices;
    }

    void WindowDestroy()
    {
        glfwDestroyWindow( _window );
    }

    void SetWindowResolution( uint16_t width, uint16_t height )
    {
        _getScreenResolution( DisplayWidth, DisplayHeight );
        if( width + height )
        {
            glfwSetWindowAttrib( _window, GLFW_RESIZABLE, GLFW_TRUE );
            glfwSetWindowAttrib( _window, GLFW_DECORATED, GLFW_TRUE );
        }
        else
        {

            glfwSetWindowAttrib( _window, GLFW_RESIZABLE, GLFW_FALSE );
            glfwSetWindowAttrib( _window, GLFW_DECORATED, GLFW_FALSE );
        }
        if( !width ) width = DisplayWidth;
        if( !height ) height = DisplayHeight;
        _width  = width;
        _height = height;
        glfwSetWindowSize( _window, static_cast<int>( width ), static_cast<int>( height ) );
    }

    void CentralizeWindow()
    {
        glfwSetWindowPos( _window, ( DisplayWidth / 2 ) - ( _width / 2 ), ( DisplayHeight / 2 ) - ( _height / 2 ) );
    }

    void SetWindowTitle( const char *new_title )
    {
        _title = new_title;
        glfwSetWindowTitle( _window, new_title );
    }
    inline void SetWindowTitle( std::string &new_title )
    {
        SetWindowTitle( new_title.c_str() );
    }

    KeyEventCallBack SetKeyEventsCallback( KeyEventCallBack Callback )
    {
        KeyEventCallBack prvsClbck{ _KeyEventCallBack };
        glfwSetKeyCallback( _window, _KeyCallbackManager );
        _KeyEventCallBack = Callback;
        return _KeyEventCallBack;
    }

    inline bool WindowShouldClose()
    {
        return glfwWindowShouldClose( _window );
    }
    inline void UpdateEvents()
    {
        glfwPollEvents();
    }

} // namespace Engine
