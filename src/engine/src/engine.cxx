#include <engine.hxx>
#include <map>
#include <RHI.hxx>
#include <device.hxx>
#include <swapchain.hxx>
#include <texture.hxx>
#include <model.hxx>
#include <renderpass.hxx>

#define CHECK_FOR_INIT assert( inited )

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
    }
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
        tools::createSwapchain();
        tools::createRenderPass();
        inited = true;
    }

    void shutdown()
    {
        tools::destroyRenderPass();
        tools::destroySwapchain();
        tools::destroyDevice();
        tools::destroySurface();
        tools::destroyWindow();
        tools::destroyDebugLayerCallback();
        tools::destroyInstance();
    }

    textureID CreateTexture( const char *path )
    {
        CHECK_FOR_INIT;
        return ( new tools::texture( path ) )->getID();
    };

    modelID CreateModel( modelType, const char *path )
    {
        CHECK_FOR_INIT;
        return ( new tools::model( path ) )->getID();
    }

    void ModelBindTexture( modelID model, textureID texture )
    {
        CHECK_FOR_INIT;
        tools::getModel( model ).setTexture( texture );
    }
} // namespace Engine