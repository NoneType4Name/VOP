#include <platform.hxx>
#include <engine.hxx>
#include <EHI.hxx>
#include <surface.hxx>
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
    instance::instance()
    {
        DEFINE_DATA_FIELD;
    }

    void instance::init( const char *appName, uint32_t appVersion )
    {
        if ( data->handle )
        {
            SPDLOG_CRITICAL( "Allready inited." );
            return;
        }
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
        std::vector<VkValidationFeatureEnableEXT> enabledVFeatures { VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT };
        ValidationFeatures.sType                          = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
        ValidationFeatures.enabledValidationFeatureCount  = enabledVFeatures.size();
        ValidationFeatures.pEnabledValidationFeatures     = enabledVFeatures.data();
        ValidationFeatures.disabledValidationFeatureCount = 0;
        ValidationFeatures.pDisabledValidationFeatures    = nullptr;

        std::vector<const char *> ext {};
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
        data->initDebugLayerCallBack( debugUtilsMsg );
    }

    void instance::DATA_TYPE::create( VkInstanceCreateInfo createInfo )
    {
        const_cast<VkApplicationInfo *>( createInfo.pApplicationInfo )->engineVersion = ENGINE_VERSION;
        const_cast<VkApplicationInfo *>( createInfo.pApplicationInfo )->pEngineName   = "NoneTypeName's Engine";
        std::vector<const char *> _layers { createInfo.ppEnabledLayerNames, &createInfo.ppEnabledLayerNames[ createInfo.enabledLayerCount ] };
        std::vector<const char *> _extensions { createInfo.ppEnabledExtensionNames, &createInfo.ppEnabledExtensionNames[ createInfo.enabledExtensionCount ] };
        setLayers( _layers );
        setExtensions( _extensions );
        assert( supportExtensions() );
        assert( supportLayers() );
        createInfo.enabledLayerCount       = layers.size();
        createInfo.ppEnabledLayerNames     = layers.data();
        createInfo.enabledExtensionCount   = extensions.size();
        createInfo.ppEnabledExtensionNames = extensions.data();
        CHECK_RESULT( vkCreateInstance( &createInfo, ALLOCATION_CALLBACK, &handle ) );
    }

    window::types::window instance::DATA_TYPE::regWindow( window::types::window window )
    {
        window->setup();
        return windows.emplace_back( window ).get();
    }

    instance::DATA_TYPE::~DATA_TYPE()
    {
    }

    window::types::window instance::createWindow( ENGINE_RESOLUTION_TYPE width, ENGINE_RESOLUTION_TYPE height, const char *title, int fullScreenRefreshRate, bool resize )
    {
        return data->regWindow( new window::window { this, { width, height, title, fullScreenRefreshRate, resize } } );
    }

    // window::types::window Engine::instance::createWindow( ENGINE_RESOLUTION_TYPE width, ENGINE_RESOLUTION_TYPE height, const char *title )
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
        data->windows.clear();
        data->destroyDebugLayerCallback();
        vkDestroyInstance( data->handle, ALLOCATION_CALLBACK );
    }
} // namespace Engine