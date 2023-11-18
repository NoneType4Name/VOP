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
    instance::instance( const char *appName, uint32_t appVersion )
    {
        DEFINE_DATA_FIELD
        VkApplicationInfo ApplicationInfo {};
        ApplicationInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        ApplicationInfo.engineVersion      = ENGINE_VERSION;
        ApplicationInfo.pEngineName        = "NoneTypeName's Engine";
        ApplicationInfo.apiVersion         = VK_API_VERSION_1_0;
        ApplicationInfo.pApplicationName   = appName;
        ApplicationInfo.applicationVersion = appVersion;

        std::vector<const char *> Extensions, Layers;
        data->setupLayers( Layers );
        data->setupExtensions( Extensions );
        data->setLayers( Layers );
        data->setExtensions( Extensions );
        data->layers.reserve( Layers.size() );
        Extensions.reserve( data->extensions.size() );
        Layers.reserve( data->layers.size() );
        for ( const auto &layer : data->layers )
            Layers.push_back( layer.data() );
        for ( const auto &extension : data->extensions )
            Extensions.push_back( extension.data() );
        assert( data->supportExtensions() );
        assert( data->supportLayers() );

        std::vector<void *> nextChainData;
        VkInstanceCreateInfo InstanceCreateInfo {};
        InstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        data->setupNextChain( InstanceCreateInfo.pNext, nextChainData );
        VkValidationFeaturesEXT *d                 = static_cast<VkValidationFeaturesEXT *>( const_cast<void *>( InstanceCreateInfo.pNext ) );
        InstanceCreateInfo.enabledLayerCount       = Layers.size();
        InstanceCreateInfo.ppEnabledLayerNames     = Layers.size() ? Layers.data() : nullptr;
        InstanceCreateInfo.enabledExtensionCount   = Extensions.size();
        InstanceCreateInfo.ppEnabledExtensionNames = Extensions.size() ? Extensions.data() : nullptr;
        InstanceCreateInfo.pApplicationInfo        = &ApplicationInfo;
        CHECK_RESULT( vkCreateInstance( &InstanceCreateInfo, ALLOCATION_CALLBACK, &data->handle ) );
        data->setupDebugLayerCallback();
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

    void instance::DATA_TYPE::setupExtensions( std::vector<const char *> &rExtensions ) {}
    void instance::DATA_TYPE::setupLayers( std::vector<const char *> &rLayers ) {}
    void instance::DATA_TYPE::setupNextChain( const void *&pNext, std::vector<void *> &dataPointers )
    {
        dataPointers.resize( 2 );
        dataPointers[ 0 ] = static_cast<void *>( new VkValidationFeatureEnableEXT { VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT } );
        dataPointers[ 1 ] = static_cast<void *>( new VkValidationFeaturesEXT {} );
        VkValidationFeaturesEXT &ValidationFeatures { *static_cast<VkValidationFeaturesEXT *>( dataPointers[ 1 ] ) };
        pNext                                             = dataPointers[ 1 ];
        ValidationFeatures.sType                          = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
        ValidationFeatures.enabledValidationFeatureCount  = 1;
        ValidationFeatures.pEnabledValidationFeatures     = static_cast<VkValidationFeatureEnableEXT *>( dataPointers[ 0 ] );
        ValidationFeatures.disabledValidationFeatureCount = 0;
        ValidationFeatures.pDisabledValidationFeatures    = nullptr;
    }

    instance::~instance()
    {
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