#include <common/logging.hxx>
#include <common/globals.hxx>
#include <platform.hxx>
#include <image.hxx>

namespace Engine
{
    namespace tools
    {
        struct SwapchainProperties
        {
            VkSurfaceCapabilitiesKHR Capabilities;
            std::vector<VkSurfaceFormatKHR> Format;
            std::vector<VkPresentModeKHR> PresentModes;
        };

        struct SwapchainImage
        {
            imageID image;
            VkSemaphore isAvailable;
            VkSemaphore isRendered;
        };

        void createSwapchain();
        void reCreateSwapchain();
        void destroySwapchain();
        uint32_t AcquireImageIndex( VkSemaphore &semaphore );
        void swapchainPresent( VkSemaphore *semaphore );
        SwapchainProperties getSwapchainProperties();
        VkSurfaceFormatKHR getSwapchainSurfaceFormat();
        VkPresentModeKHR getSwapchainSurfacePresentMode();
        VkFormat getSwapchainDepthImageFormat();
    } // namespace tools
} // namespace Engine