#include <common/logging.hxx>
#include <common/globals.hxx>
#include <platform.hxx>
#include <image.hxx>
#include <device.hxx>

namespace Engine
{
    struct swapchain::DATA_TYPE
    {
        DATA_TYPE( types::swapchain parent, types::device device, window::types::window window );

        struct properties_T
        {
            properties_T( types::device device, window::types::window window );
            VkSurfaceCapabilitiesKHR capabilities {};
            std::vector<VkSurfaceFormatKHR> formats;
            std::vector<VkPresentModeKHR> presentModes;
        };

        struct image_T
        {
            types::image image { nullptr };
            VkSemaphore isAvailable { nullptr };
            VkSemaphore isRendered { nullptr };
        };

        void create( VkSwapchainCreateInfoKHR createInfo );
        ~DATA_TYPE();

        window::types::window window { nullptr };
        types::device device { nullptr };
        VkSwapchainKHR handle { nullptr };
        VkSurfaceFormatKHR format { VK_FORMAT_MAX_ENUM };
        VkPresentModeKHR presentMode { VK_PRESENT_MODE_MAX_ENUM_KHR };
        VkFormat depthImageFormat { VK_FORMAT_MAX_ENUM };
        properties_T properties;
        std::vector<image_T> images;
        uint32_t flightImgIndex { 0 };
        uint32_t semaphoreIndex { 0 };
        types::swapchain parent { nullptr };
    };

    // namespace tools
    // {
    //     VkFormat formatPriority( types::device device, const std::vector<VkFormat> &formats, VkImageTiling ImageTiling, VkFormatFeatureFlags FormatFeatureFlags );
    // } // namespace tools
} // namespace Engine