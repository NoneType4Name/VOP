#include "WindowsPlatform.hxx"
#include <common/logging.hxx>
#include <surface.hxx>
#include <instance.hxx>

namespace Engine
{
    namespace window
    {
        void window::DATA_TYPE::createSurface( const void *pNext, VkWin32SurfaceCreateFlagsKHR flags )
        {
            VkWin32SurfaceCreateInfoKHR CI { VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR, pNext, flags, GetModuleHandle( nullptr ), glfwGetWin32Window( parent->glfwHandle ) };
            CHECK_RESULT( vkCreateWin32SurfaceKHR( this->instance->handle, &CI, ENGINE_ALLOCATION_CALLBACK, &parent->surface ) );
        };
    } // namespace window
} // namespace Engine
