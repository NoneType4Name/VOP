#include <common/globals.hxx>
#include <common/logging.hxx>
#include <platform.hxx>

#define ENGINE_VERSION VK_MAKE_VERSION( 0, 0, 1 )

namespace Engine
{
    namespace tools
    {
        // struct swapChain
        // {
        //     VkSwapchainKHR Swapchain;
        //     VkSurfaceFormatKHR Format{ VK_FORMAT_UNDEFINED };
        //     VkPresentModeKHR PresentMode;
        //     VkSurfaceCapabilitiesKHR Capabilities;
        //     std::vector<VkSurfaceFormatKHR> AviliableFormats;
        //     std::vector<VkPresentModeKHR> AviliablePresentModes;
        // };

        // #ifdef ENGINE_DEBUG
        void setupDebugLayerCallback();
        void destroyDebugLayerCallback();
        // #endif
        void createInstance();
        void destroyInstance();
        void createSwapchain();
        void ReCreateSwapchain();
        void destroySwapchain();
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
