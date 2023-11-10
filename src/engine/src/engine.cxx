#include <engine.hxx>
#include <map>
#include <EHI.hxx>
// #include <RHI.hxx>
#include <surface.hxx>
#include <device.hxx>
#include <swapchain.hxx>
#include <texture.hxx>
#include <model.hxx>
#include <renderpass.hxx>
#include <descriptorSet.hxx>
#include <pipeline.hxx>
#include <sampler.hxx>

#define CHECK_FOR_INIT assert( inited )

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
    instance::instance( const char *appName, uint32_t appVersion )
    {
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
        data->layers.clear();
        data->extensions.clear();
        data->layers.reserve( Layers.size() );
        data->extensions.reserve( Extensions.size() );
        // data->validationFeatures.reserve( Validation.size() );
        for ( auto layer : Layers )
            data->layers.push_back( layer );
        for ( auto extension : Extensions )
            data->extensions.push_back( extension );
        Extensions.clear();
        Layers.clear();
        assert( data->supportExtensions() );
        assert( data->supportLayers() );

        VkInstanceCreateInfo InstanceCreateInfo {};
        InstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        data->setupNextChain( InstanceCreateInfo.pNext );
        InstanceCreateInfo.enabledLayerCount       = Layers.size();
        InstanceCreateInfo.ppEnabledLayerNames     = Layers.size() ? Layers.data() : nullptr;
        InstanceCreateInfo.enabledExtensionCount   = Extensions.size();
        InstanceCreateInfo.ppEnabledExtensionNames = Extensions.size() ? Extensions.data() : nullptr;
        InstanceCreateInfo.pApplicationInfo        = &ApplicationInfo;
        CHECK_RESULT( vkCreateInstance( &InstanceCreateInfo, ALLOCATION_CALLBACK, &data->handle ) );
        data->setupDebugLayerCallback();
    }
#ifdef CreateWindow
#    undef CreateWindow
#endif
    window::types::window Engine::instance::CreateWindow( RESOLUTION_TYPE width, RESOLUTION_TYPE height, const char *title )
    {
        data->windows.push_back( std::unique_ptr<window::window> { new window::window { width, height, title, this } } );
        return data->windows.back().get();
    }

    window::types::window Engine::instance::CreateWindow( RESOLUTION_TYPE width, RESOLUTION_TYPE height, std::string title )
    {
        CreateWindow( width, height, title.data() );
    }

    types::device instance::CreateDevice( types::DeviceDescription description )
    {
        data->devices.push_back( std::unique_ptr<device> { new device { description } } );
        return data->devices.back().get();
    }

    types::link instance::CreateLink( window::types::window window, types::device device )
    {
        data->links.push_back( std::unique_ptr<link> { new link { window, device } } );
        return data->links.back().get();
    }

    // ! // void instance::init()
    //   // {
    //   // }

#ifdef UNICODE
#    define CreateWindow CreateWindowW
#else
#    define CreateWindow CreateWindowA
#endif

    instance::~instance()
    {
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