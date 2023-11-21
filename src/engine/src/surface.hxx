#pragma once
#include <common/globals.hxx>
#include <common/logging.hxx>
#include <platform.hxx>

namespace Engine
{
    struct window::window::DATA_TYPE
    {
        virtual void setupNextChain( const void *&pNext, std::vector<void *> &dataPointers, void *userPoiner );
        virtual void setupFlags( VkWin32SurfaceCreateFlagsKHR flags, void *userPoiner );
        void createSurface( VkInstance instance, const void *pNext, VkWin32SurfaceCreateFlagsKHR flags );
        void destroySurface();
        RESOLUTION_TYPE width { 0 };
        RESOLUTION_TYPE height { 0 };
        std::string title;
        ResizeCallback resizeCallBack { nullptr };
        KeyEventCallBack eventCallBack { nullptr };
        GLFWwindow *window { nullptr };
        VkSurfaceKHR surface { nullptr };
        Engine::types::instance instance { nullptr };
    };
} // namespace Engine