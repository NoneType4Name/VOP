#pragma once
#include <common/globals.hxx>
#include <common/logging.hxx>
#include <platform.hxx>
#include <engine.hxx>

namespace Engine
{
    namespace window
    {
        struct window::data
        {
            void createSurface( VkInstance instance, const void *pNext, VkWin32SurfaceCreateFlagsKHR flags );
            void destroySurface();
            RESOLUTION_TYPE width { 0 };
            RESOLUTION_TYPE height { 0 };
            std::string title {};
            ResizeCallback resizeCallBack { nullptr };
            KeyEventCallBack eventCallBack { nullptr };
            GLFWwindow *window { nullptr };
            VkSurfaceKHR surface { nullptr };
            VkInstance instance { nullptr };
        };
    } // namespace window
} // namespace Engine