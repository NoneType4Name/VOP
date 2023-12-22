#pragma once
#ifndef SURFACE_HXX
#    define SURFACE_HXX
#    include <common/globals.hxx>
#    include <common/logging.hxx>
#    include <platform.hxx>

namespace Engine
{
    struct window::window::DATA_TYPE
    {
        DATA_TYPE( window *parent ) :
            parent { parent } {}
        void createSurface( VkInstance instance, const void *pNext, VkFlags flags );
        void destroySurface( VkInstance instance );
        ENGINE_RESOLUTION_TYPE width { 0 };
        ENGINE_RESOLUTION_TYPE height { 0 };
        std::string title;
        ResizeCallback resizeCallBack { nullptr };
        KeyEventCallBack eventCallBack { nullptr };
        GLFWwindow *window { nullptr };
        VkSurfaceKHR surface { nullptr };
        Engine::types::instance instance { nullptr };
        settings settings;
        Engine::window::types::window parent { nullptr };
    };
} // namespace Engine
#endif