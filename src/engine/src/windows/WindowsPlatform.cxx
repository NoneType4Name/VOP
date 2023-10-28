#include "WindowsPlatform.hxx"
#include <common/logging.hxx>
#include <engine.hxx>
#include <RHI.hxx>
#include <surface.hxx>

namespace Engine
{
    namespace window
    {
        void window::data::createSurface( VkInstance instance, const void *pNext, VkWin32SurfaceCreateFlagsKHR flags )
        {
            VkWin32SurfaceCreateInfoKHR CI { VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR, 0, 0, GetModuleHandle( nullptr ), glfwGetWin32Window( window ) };
            CHECK_RESULT( vkCreateWin32SurfaceKHR( instance, &CI, ALLOCATION_CALLBACK, &surface ) );
        };

        void window::data::destroySurface()
        {
            vkDestroySurfaceKHR( instance, surface, ALLOCATION_CALLBACK );
        }
    } // namespace window
} // namespace Engine
