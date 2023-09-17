#include <engine.hxx>
#include <map>
#include <texture.hxx>
#include <RHI.hxx>
#include <device.hxx>
#include <swapchain.hxx>

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
    namespace tools
    {
        namespace
        {
            std::unordered_map<textureID, texture> _textures;
        }
    } // namespace tools
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
        tools::_textures.clear();
        tools::destroySwapchain();
        tools::destroyDevice();
        tools::destroySurface();
        tools::destroyWindow();
        tools::destroyDebugLayerCallback();
        tools::destroyInstance();
    }

    textureID CreateTexture( const char *path )
    {
        auto t                         = new tools::texture( path );
        tools::_textures[ t->GetID() ] = *t;
        return t->GetID();
    };

    modelID CreateModel( modelType, const char *path );
    void ModelBindTexture( modelID model, textureID texture );
} // namespace Engine