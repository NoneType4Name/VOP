#include <engine.hxx>
#include <EHI.hxx>
// #include <RHI.hxx>
#include <surface.hxx>
#include <device.hxx>
#include <swapchain.hxx>
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
    } _;
} // namespace

namespace Engine
{
    instance::instance() = default;
    instance::instance( const char *appName, uint32_t appVersion, void *userPoiner )
    {
        DEFINE_DATA_FIELD
        data->userPointer = userPoiner;
        VkApplicationInfo ApplicationInfo {};
        ApplicationInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        ApplicationInfo.engineVersion      = ENGINE_VERSION;
        ApplicationInfo.pEngineName        = "NoneTypeName's Engine";
        ApplicationInfo.apiVersion         = VK_API_VERSION_1_0;
        ApplicationInfo.pApplicationName   = appName;
        ApplicationInfo.applicationVersion = appVersion;

        std::vector<const char *> Extensions, Layers;
        std::vector<void *> pData;
        VkDebugUtilsMessengerCreateInfoEXT debugUtilsMsg {};
        data->setupDebugLayerCallback( debugUtilsMsg, pData );
        VkDebugUtilsMessengerCreateInfoEXT debugUtilsMsgCI = debugUtilsMsg;
        debugUtilsMsgCI.pNext                              = nullptr;
        data->setupLayers( Layers );
        data->setupExtensions( Extensions );
        data->setLayers( Layers );
        data->setExtensions( Extensions );
        assert( data->supportExtensions() );
        assert( data->supportLayers() );

        std::vector<void *> nextChainData;
        VkInstanceCreateInfo InstanceCreateInfo {};
        InstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        InstanceCreateInfo.pNext = &debugUtilsMsgCI;
        // data->setupNextChain( debugUtilsMsgCI.pNext, nextChainData );
        InstanceCreateInfo.enabledLayerCount       = data->layers.size();
        InstanceCreateInfo.ppEnabledLayerNames     = data->layers.size() ? data->layers.data() : nullptr;
        InstanceCreateInfo.enabledExtensionCount   = data->extensions.size();
        InstanceCreateInfo.ppEnabledExtensionNames = data->extensions.size() ? data->extensions.data() : nullptr;
        InstanceCreateInfo.pApplicationInfo        = &ApplicationInfo;
        CHECK_RESULT( vkCreateInstance( &InstanceCreateInfo, ALLOCATION_CALLBACK, &data->handle ) );
        data->initDebugLayerCallBack( debugUtilsMsg );
    }

    instance::DATA_TYPE::~DATA_TYPE()
    {
        // deviceDescriptions.clear();
        // links.clear();
        // windows.clear();
        // devices.clear();
    }

    window::types::window Engine::instance::createWindow( RESOLUTION_TYPE width, RESOLUTION_TYPE height, const char *title )
    {
        data->windows.push_back( std::unique_ptr<window::window> { new window::window { width, height, title, this } } );
        return data->windows.back().get();
    }

    window::types::window Engine::instance::createWindow( RESOLUTION_TYPE width, RESOLUTION_TYPE height, std::string title )
    {
        return createWindow( width, height, title.data() );
    }

    // types::device instance::CreateDevice( types::DeviceDescription description )
    // {
    //     data->devices.emplace_back( std::unique_ptr<device> { new device { description } } );
    //     return data->devices.back().get();
    // }

    types::link instance::CreateLink( window::types::window window, types::DeviceDescription description )
    {
        data->links.emplace_back( std::unique_ptr<link> { new link } );
        data->devices.emplace_back( std::unique_ptr<device> { new device { description, window } } );
        auto &_data = const_cast<std::unique_ptr<link::DATA_TYPE> &>( data->links.back()->data );
        _data.reset( new link::DATA_TYPE { window, data->devices.back().get() } );
        return data->links.back().get();
    }

    types::pass instance::CreateRenderPass( types::link link )
    {
        data->passes.emplace_back( std::unique_ptr<pass> { new pass { link } } );
        return data->passes.back().get();
    }

    void instance::DATA_TYPE::setupExtensions( std::vector<const char *> &rExtensions ) {}
    void instance::DATA_TYPE::setupLayers( std::vector<const char *> &rLayers ) {}
    void instance::DATA_TYPE::setupNextChain( const void *&pNext, std::vector<void *> &dataPointers )
    {
        dataPointers.resize( 5 );
        dataPointers[ 0 ] = static_cast<void *>( new VkValidationFeaturesEXT {} );
        dataPointers[ 1 ] = static_cast<void *>( new VkValidationFeatureEnableEXT { VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT } );
        dataPointers[ 2 ] = static_cast<void *>( new VkValidationFeatureEnableEXT { VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_EXT } );
        dataPointers[ 3 ] = static_cast<void *>( new VkValidationFeatureEnableEXT { VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_RESERVE_BINDING_SLOT_EXT } );
        dataPointers[ 4 ] = static_cast<void *>( new VkValidationFeatureEnableEXT { VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT } );
        VkValidationFeaturesEXT &ValidationFeatures { *static_cast<VkValidationFeaturesEXT *>( dataPointers[ 0 ] ) };
        ValidationFeatures.sType                          = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
        ValidationFeatures.enabledValidationFeatureCount  = 4;
        ValidationFeatures.pEnabledValidationFeatures     = static_cast<VkValidationFeatureEnableEXT *>( dataPointers[ 1 ] );
        ValidationFeatures.disabledValidationFeatureCount = 0;
        ValidationFeatures.pDisabledValidationFeatures    = nullptr;
    }

    instance::~instance()
    {
        data->passes.clear();
        data->links.clear();
        data->windows.clear();
        data->devices.clear();
        data->deviceDescriptions.clear();
        data->destroyDebugLayerCallback();
        vkDestroyInstance( data->handle, ALLOCATION_CALLBACK );
    }

    // void init( AppCreateInfo sAppCreateInfo )
    // {
    //     tools::setSettings( sAppCreateInfo );
    //     tools::createWindow( sAppCreateInfo.width, sAppCreateInfo.height, sAppCreateInfo.title );
    //     tools::createSurface( tools::getInstance() );
    //     tools::createDevice( static_cast<VkPhysicalDevice>( sAppCreateInfo.device.ptr ) );
    //     tools::createShaderModules();
    //     tools::createSwapchain();
    //     tools::createRenderPass();
    //     tools::createSamplers();
    //     tools::createDescriptorPool();
    //     tools::createPipelines();
    //     inited = true;
    // }

    // void shutdown()
    // {
    //     tools::destroyPipelines();
    //     tools::destroyDescriptorPool();
    //     tools::destroyRenderPass();
    //     tools::destroySwapchain();
    //     tools::destroyShaderModules();
    //     tools::destroyDevice();
    //     tools::destroySurface();
    //     tools::destroyWindow();
    //     tools::destroyDebugLayerCallback();
    //     tools::destroyInstance();
    //     inited = false;
    // }

    // textureID CreateTexture( const char *path )
    // {
    //     CHECK_FOR_INIT;
    //     auto d = inited;
    //     return ( new tools::texture( path ) )->getID();
    // };

    // modelID CreateModel( modelType, const char *path )
    // {
    //     CHECK_FOR_INIT;
    //     return ( new tools::model( path ) )->getID();
    // }

    // shaderID CreateShader( const char *path, const char *mainFuncName, ShaderStage stage )
    // {
    //     return ( new tools::shader { path, mainFuncName, stage } )->getID();
    // }

    // pipelineID CreatePipeline( PipelineInfo info )
    // {
    //     return ( new tools::pipeline { info } )->getID();
    // }

    // void ModelBindTexture( modelID model, textureID texture )
    // {
    //     CHECK_FOR_INIT;
    //     tools::getModel( model )->setTexture( texture );
    // }
} // namespace Engine