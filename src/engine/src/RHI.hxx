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
        void destoroyDebugLayerCallback();
        // #endif
        void createInstance();
        void destroyInstance();
        void createSwapchain();
        void ReCreateSwapchain();
        void destroySwapchain();
        std::vector<VkPhysicalDevice> getAviliableDevices();
        std::vector<VkPhysicalDeviceProperties> getAviliableDevicesProperties();
        void getDeviceLayersAndExtension( std::vector<const char *> &deviceLayers, std::vector<const char *> &deviceExtensions );
        void getInstanceLayersAndExtension( std::vector<const char *> &instanceLayers, std::vector<const char *> &instanceExtensions );
        bool instanseSupportExtensionsAndLayers( std::vector<const char *> extensions, std::vector<const char *> layers );
        bool deviceSupportExtensionsAndLayers( std::vector<const char *> extensions, std::vector<const char *> layers );
        VkInstance getInstance();
        VkSwapchainKHR getSwapchain();
    } // namespace tools
} // namespace Engine
