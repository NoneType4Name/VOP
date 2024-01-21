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
        ~DATA_TYPE();
        void create( VkSwapchainCreateInfoKHR createInfo );

        window::types::window window { nullptr };
        types::device device { nullptr };
        uint32_t flightImgIndex { 0 };
        uint32_t semaphoreIndex { 0 };
        types::swapchain parent { nullptr };
    };

    // namespace tools
    // {
    //     VkFormat formatPriority( types::device device, const std::vector<VkFormat> &formats, VkImageTiling ImageTiling, VkFormatFeatureFlags FormatFeatureFlags );
    // } // namespace tools
} // namespace Engine