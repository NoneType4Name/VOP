#include <engine.hxx>
#include <RHI.hxx>
#include <device.hxx>

namespace
{
    struct __init
    {
        __init()
        {
            Engine::tools::createInstance();
        }
        ~__init()
        {
            Engine::tools::destroyInstance();
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
        tools::createInstance();
        tools::createWindow( sAppCreateInfo.width, sAppCreateInfo.height, sAppCreateInfo.title );
        tools::createDevice( static_cast<VkPhysicalDevice>( sAppCreateInfo.device.ptr ) );
    }

    void shutdown()
    {
        tools::destroyWindow();
        tools::destroyDevice();
    }
} // namespace Engine