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
        DATA_TYPE( types::window parent, instance *instance );
        void createSurface( const void *pNext, VkFlags flags );
        ~DATA_TYPE();
        std::set<Engine::types::swapchain> swapchains;
        Engine::instance *instance;
        types::window parent;
    };
} // namespace Engine
#endif