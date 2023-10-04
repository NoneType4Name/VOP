#include <engine.hxx>
#include <map>
#include <RHI.hxx>
#include <device.hxx>
#include <swapchain.hxx>
#include <texture.hxx>
#include <model.hxx>
#include <renderpass.hxx>
#include <descriptorSet.hxx>
#include <pipeline.hxx>

#define CHECK_FOR_INIT assert( !inited )

namespace
{
    struct __init
    {
        __init()
        {
            assert( glfwInit() );
            Engine::tools::createInstance();
            Engine::tools::setupDebugLayerCallback();
        }
    } _;
} // namespace

namespace Engine
{
    namespace
    {
        bool inited{ false };
        tools::descriptorSetID _defaultDescriptorSetID;
    } // namespace
    std::vector<Device> GetGraphicDevices( uint8_t devicesTypeFlag )
    {
        std::vector<Device> devices{};
        tools::getSuitableDevices( devices, devicesTypeFlag );
        return devices;
    }

    void GetActiveGrapchicDevice( Device &device )
    {
        VkPhysicalDeviceProperties pr{};
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
        _defaultDescriptorSetID = ( new tools::descriptorSet{
                                        { { 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS, 1 },
                                          { 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1 } } } )
                                      ->getID();
        tools::createDescriptorPool();
        tools::createPipelines();
        inited = true;
    }

    void shutdown()
    {
        tools::destroyPipelines();
        delete &tools::getDescriptorSet( _defaultDescriptorSetID );
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
        return ( new tools::shader{ path, mainFuncName, stage } )->getID();
    }

    pipelineID CreatePipeline( PipelineInfo info )
    {
        return ( new tools::pipeline{ info } )->getID();
    }

    void ModelBindTexture( modelID model, textureID texture )
    {
        CHECK_FOR_INIT;
        tools::getModel( model ).setTexture( texture );
    }
} // namespace Engine