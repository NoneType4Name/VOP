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
        GLFWwindow *window { nullptr };
        VkSurfaceKHR surface { nullptr };
        Engine::types::instance instance { nullptr };
        Engine::window::types::window parent { nullptr };
    };
} // namespace Engine
#endif