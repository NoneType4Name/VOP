#pragma once
#ifndef SURFACE_HXX
#    define SURFACE_HXX
#    include <common/globals.hxx>

namespace Engine
{
    struct surface::DATA_TYPE
    {
        DATA_TYPE( types::surface parent, instance *instance, uint32_t width, uint32_t height );
        ~DATA_TYPE();
        uint32_t width;
        uint32_t height;
        std::set<Engine::types::swapchain> swapchains;
        Engine::instance *instance;
        types::surface parent;
    };
} // namespace Engine
#endif