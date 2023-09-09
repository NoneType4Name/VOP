#include <common/globals.hxx>
#include <common/logging.hxx>
#include <platform.hxx>

#define ENGINE_VERSION VK_MAKE_VERSION( 0, 0, 1 )

namespace Engine
{
    namespace tools
    {
        // #ifdef ENGINE_DEBUG
        void setupDebugLayerCallback();
        void destroyDebugLayerCallback();
        // #endif
        void createInstance();
        void destroyInstance();
        std::vector<VkPhysicalDevice> getAviliableDevices();
        std::vector<VkPhysicalDeviceProperties> getAviliableDevicesProperties();
        void getLayers( std::vector<const char *> &layers );
        void getDeviceExtensions( std::vector<const char *> &deviceExtensions );
        void getInstanceExtensions( std::vector<const char *> &instanceExtensions );
        bool isInstanseSupportExtensions( std::vector<const char *> extensions );
        bool isDeviceSupportExtensions( std::vector<const char *> extensions );
        VkInstance getInstance();
        VkSwapchainKHR getSwapchain();
    } // namespace tools
} // namespace Engine
