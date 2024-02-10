#include <common/globals.hxx>

#include <image.hxx>
#include <device.hxx>

namespace Engine
{
    struct swapchain::DATA_TYPE
    {
        DATA_TYPE( types::swapchain parent, types::device device, types::surface surface );
        ~DATA_TYPE();
        void create( VkSwapchainCreateInfoKHR createInfo );

        types::surface surface { nullptr };
        types::device device { nullptr };
        uint32_t flightImgIndex { 0 };
        uint32_t semaphoreIndex { 0 };
        types::swapchain parent { nullptr };
    };
} // namespace Engine