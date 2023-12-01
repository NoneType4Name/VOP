#include <common/logging.hxx>
#include <common/globals.hxx>
#include <platform.hxx>
#include <image.hxx>

namespace Engine
{
    struct link::DATA_TYPE
    {
        DATA_TYPE( window::types::window window, types::device device );

        struct properties_T
        {
            properties_T( window::types::window window, types::device device );
            VkSurfaceCapabilitiesKHR capabilities {};
            std::vector<VkSurfaceFormatKHR> formats;
            std::vector<VkPresentModeKHR> presentModes;
        };

        struct image_T
        {
            std::unique_ptr<image> image { nullptr };
            VkSemaphore isAvailable { nullptr };
            VkSemaphore isRendered { nullptr };
        };
        void setup( types::link swapchain, VkSwapchainCreateInfoKHR &createInfo, std::vector<std::unique_ptr<void>> &userData, void *userPoiner );
        void setupImgs();
        ~DATA_TYPE();

        window::types::window window { nullptr };
        types::device device { nullptr };
        VkSwapchainKHR swapchain { nullptr };
        VkSurfaceFormatKHR format { VK_FORMAT_MAX_ENUM };
        VkPresentModeKHR presentMode { VK_PRESENT_MODE_MAX_ENUM_KHR };
        VkFormat depthImageFormat { VK_FORMAT_MAX_ENUM };
        properties_T properties;
        std::vector<image_T> images;
        uint32_t flightImgIndex { 0 };
        uint32_t semaphoreIndex { 0 };
        VkSwapchainCreateInfoKHR createInfo {};
    };
} // namespace Engine