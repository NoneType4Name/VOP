#include <engine.hxx>
#include <map>
#include <RHI.hxx>
#include <device.hxx>
#include <swapchain.hxx>
#include <texture.hxx>
#include <model.hxx>

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
    std::vector<Device> GetGraphicDevices( uint8_t devicesTypeFlag )
    {
        std::vector<Device> devices{};
        tools::getSuitableDevices( devices, devicesTypeFlag );
        return devices;
    }

    void GetActiveGrapchiDevice( Device &device )
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
    }

    void shutdown()
    {
        tools::destroySwapchain();
        tools::destroyDevice();
        tools::destroySurface();
        tools::destroyWindow();
        tools::destroyDebugLayerCallback();
        tools::destroyInstance();
    }

    textureID CreateTexture( const char *path )
    {
        return ( new tools::texture( path ) )->getID();
    };

    modelID CreateModel( modelType, const char *path )
    {
        return ( new tools::model( path ) )->getID();
    }

    void ModelBindTexture( modelID model, textureID texture )
    {
        tools::getModel( model ).setTexture( texture );
    }

    shadersLayoutID CreateShadersLayout( ShadersLayout layout )
    {
    }
} // namespace Engine