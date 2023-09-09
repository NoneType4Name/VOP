#include <common/logging.hxx>
#include <common/globals.hxx>
#include <platform.hxx>

namespace Engine
{
    namespace tools
    {
        void createSwapchain();
        void reCreateSwapchain();
        void destroySwapchain();
        VkSurfaceFormatKHR getSwapchainSurfaceFormat();
        VkPresentModeKHR getSwapchainSurfacePresentMode();
        VkFormat getSwapchainDepthImageFormat();
    } // namespace tools
} // namespace Engine