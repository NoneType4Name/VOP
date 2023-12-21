#include <platform.hxx>
#include <engine.hxx>
#include <EHI.hxx>
// #include <RHI.hxx>
// #include <surface.hxx>
// #include <device.hxx>
// #include <swapchain.hxx>
// #include <texture.hxx>
// #include <model.hxx>
// #include <renderpass.hxx>
// #include <descriptorSet.hxx>
// #include <pipeline.hxx>
// #include <sampler.hxx>

namespace
{
    struct __init
    {
        __init()
        {
            glfwSetErrorCallback( []( int code, const char *data )
                                  { SPDLOG_CRITICAL( "GLFW ERROR {}: {}", code, data ); } );
            assert( glfwInit() );
        }
        ~__init()
        {
            glfwTerminate();
        }
    } _;
} // namespace

namespace Engine
{
    instance::instance() = default;
    instance::instance( const char *appName, uint32_t appVersion )
    {
        DEFINE_DATA_FIELD
        setup( appName, appVersion );
    }

    void instance::setup( const char *appName, uint32_t appVersion )
    {
        VkApplicationInfo ApplicationInfo {};
        ApplicationInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        ApplicationInfo.apiVersion         = VK_API_VERSION_1_0;
        ApplicationInfo.pApplicationName   = appName;
        ApplicationInfo.applicationVersion = appVersion;

        VkDebugUtilsMessengerCreateInfoEXT debugUtilsMsg {};
        data->setupDebugLayerCallback( debugUtilsMsg );
        VkDebugUtilsMessengerCreateInfoEXT debugUtilsMsgCI = debugUtilsMsg;
        VkValidationFeaturesEXT ValidationFeatures {};
        debugUtilsMsgCI.pNext = &ValidationFeatures;
        std::vector<VkValidationFeatureEnableEXT> enabledVFeatures { VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT,
                                                                     VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_EXT,
                                                                     VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_RESERVE_BINDING_SLOT_EXT,
                                                                     VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT };
        ValidationFeatures.sType                          = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
        ValidationFeatures.enabledValidationFeatureCount  = enabledVFeatures.size();
        ValidationFeatures.pEnabledValidationFeatures     = enabledVFeatures.data();
        ValidationFeatures.disabledValidationFeatureCount = 0;
        ValidationFeatures.pDisabledValidationFeatures    = nullptr;

        VkInstanceCreateInfo InstanceCreateInfo {};
        InstanceCreateInfo.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        InstanceCreateInfo.pNext                   = &debugUtilsMsgCI;
        InstanceCreateInfo.enabledLayerCount       = 0;
        InstanceCreateInfo.ppEnabledLayerNames     = nullptr;
        InstanceCreateInfo.enabledExtensionCount   = 0;
        InstanceCreateInfo.ppEnabledExtensionNames = nullptr;
        InstanceCreateInfo.pApplicationInfo        = &ApplicationInfo;
        data->init( InstanceCreateInfo );
        data->initDebugLayerCallBack( debugUtilsMsg );
    }

    void instance::DATA_TYPE::init( VkInstanceCreateInfo createInfo )
    {
        const_cast<VkApplicationInfo *>( createInfo.pApplicationInfo )->engineVersion = ENGINE_VERSION;
        const_cast<VkApplicationInfo *>( createInfo.pApplicationInfo )->pEngineName   = "NoneTypeName's Engine";
        std::vector<const char *> _layers { createInfo.ppEnabledLayerNames, &createInfo.ppEnabledLayerNames[ createInfo.enabledLayerCount ] };
        std::vector<const char *> _extensions { createInfo.ppEnabledExtensionNames, &createInfo.ppEnabledExtensionNames[ createInfo.enabledExtensionCount ] };
        setLayers( _layers );
        setExtensions( _extensions );
        assert( supportExtensions() );
        assert( supportLayers() );
        CHECK_RESULT( vkCreateInstance( &createInfo, ALLOCATION_CALLBACK, &handle ) );
    }

    instance::DATA_TYPE::~DATA_TYPE()
    {
    }

    // window::types::window instance::DATA_TYPE::createWindow( RESOLUTION_TYPE width, RESOLUTION_TYPE height, const char *title )
    // {
    //     return new window::window { this->parent, width, height, title };
    // }

    // window::types::window Engine::instance::createWindow( RESOLUTION_TYPE width, RESOLUTION_TYPE height, const char *title )
    // {
    //     return data->windows.emplace_back( new window::window { this, width, height, title } ).get();
    // }

    // std::pair<types::link, types::device> instance::makeLink( window::types::window window, types::DeviceDescription description )
    // {
    //     data->devices.emplace_back( std::unique_ptr<device> { new device { description, window } } );
    //     data->links.emplace_back( std::unique_ptr<link> { new link { window, data->devices.back().get() } } );
    //     // auto &_data = const_cast<std::unique_ptr<link::DATA_TYPE> &>( data->links.back()->data );
    //     // _data.reset( new link::DATA_TYPE { window, data->devices.back().get() } );
    //     return { data->links.back().get(), data->devices.back().get() };
    // }

    // types::pass instance::CreateRenderPass( types::link link )
    // {
    //     data->passes.emplace_back( std::unique_ptr<pass> { new pass { link } } );
    //     return data->passes.back().get();
    // }

    instance::~instance()
    {
        // data->passes.clear();
        // data->links.clear();
        // data->windows.clear();
        // data->devices.clear();
        // data->deviceDescriptions.clear();
        data->destroyDebugLayerCallback();
        vkDestroyInstance( data->handle, ALLOCATION_CALLBACK );
    }
} // namespace Engine