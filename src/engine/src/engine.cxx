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
    Instance::Instance( const char *appName, uint32_t appVersion )
    {
        VkApplicationInfo ApplicationInfo {};
        ApplicationInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        ApplicationInfo.engineVersion      = ENGINE_VERSION;
        ApplicationInfo.pEngineName        = "NoneTypeName's Engine";
        ApplicationInfo.apiVersion         = VK_API_VERSION_1_0;
        ApplicationInfo.pApplicationName   = appName;
        ApplicationInfo.applicationVersion = appVersion;

        std::vector<const char *> Extensions, Layers;
        std::vector<VkValidationFeatureEnableEXT> Validation;
        setupExtensions( Extensions );
        setupLayers( Layers );
        data->setValidationFeatures( Validation );
        data->setExtensions( Extensions );
        data->setLayers( Extensions );
        Extensions.clear();
        Layers.clear();
        Validation.clear();
        assert( data->supportExtensions() );
        assert( data->supportLayers() );

        VkValidationFeaturesEXT ValidationFeatures {};
        ValidationFeatures.sType                         = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
        ValidationFeatures.enabledValidationFeatureCount = data->validationFeatures.size();
        ValidationFeatures.pEnabledValidationFeatures    = data->validationFeatures.data();

        VkInstanceCreateInfo InstanceCreateInfo {};
        InstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        // #ifdef ENGINE_DEBUG
        InstanceCreateInfo.pNext = &ValidationFeatures;
        // #endif
        InstanceCreateInfo.enabledLayerCount       = Layers.size();
        InstanceCreateInfo.ppEnabledLayerNames     = Layers.size() ? Layers.data() : nullptr;
        InstanceCreateInfo.enabledExtensionCount   = Extensions.size();
        InstanceCreateInfo.ppEnabledExtensionNames = Extensions.size() ? Extensions.data() : nullptr;
        InstanceCreateInfo.pApplicationInfo        = &ApplicationInfo;
        CHECK_RESULT( vkCreateInstance( &InstanceCreateInfo, ALLOCATION_CALLBACK, &data->handle ) );
        data->setupDebugLayerCallback();
    }

    Instance::~Instance()
    {
        data->destroyDebugLayerCallback();
        vkDestroyInstance( data->handle, ALLOCATION_CALLBACK );
    }

    std::vector<Device> GetGraphicDevices( uint8_t devicesTypeFlag )
    {
        std::vector<Device> devices {};
        tools::getSuitableDevices( devices, devicesTypeFlag );
        return devices;
    }

    void GetActiveGrapchicDevice( Device &device )
    {
        VkPhysicalDeviceProperties pr {};
        vkGetPhysicalDeviceProperties( tools::getPhysicalDevice(), &pr );
        device.name = pr.deviceName;
        device.ptr  = &device;
    }

    void init( AppCreateInfo sAppCreateInfo )
    {
        tools::setSettings( sAppCreateInfo );
        tools::createWindow( sAppCreateInfo.width, sAppCreateInfo.height, sAppCreateInfo.title );
        tools::createSurface( tools::getInstance() );
        tools::createDevice( static_cast<VkPhysicalDevice>( sAppCreateInfo.device.ptr ) );
        tools::createShaderModules();
        tools::createSwapchain();
        tools::createRenderPass();
        tools::createSamplers();
        tools::createDescriptorPool();
        tools::createPipelines();
        inited = true;
    }

    void shutdown()
    {
        tools::destroyPipelines();
        tools::destroyDescriptorPool();
        tools::destroyRenderPass();
        tools::destroySwapchain();
        tools::destroyShaderModules();
        tools::destroyDevice();
        tools::destroySurface();
        tools::destroyWindow();
        tools::destroyDebugLayerCallback();
        tools::destroyInstance();
        inited = false;
    }

    textureID CreateTexture( const char *path )
    {
        CHECK_FOR_INIT;
        auto d = inited;
        return ( new tools::texture( path ) )->getID();
    };

    modelID CreateModel( modelType, const char *path )
    {
        CHECK_FOR_INIT;
        return ( new tools::model( path ) )->getID();
    }

    shaderID CreateShader( const char *path, const char *mainFuncName, ShaderStage stage )
    {
        return ( new tools::shader { path, mainFuncName, stage } )->getID();
    }

    pipelineID CreatePipeline( PipelineInfo info )
    {
        return ( new tools::pipeline { info } )->getID();
    }

    void ModelBindTexture( modelID model, textureID texture )
    {
        CHECK_FOR_INIT;
        tools::getModel( model )->setTexture( texture );
    }
} // namespace Engine