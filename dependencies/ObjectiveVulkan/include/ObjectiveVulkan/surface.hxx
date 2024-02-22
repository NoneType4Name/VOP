#pragma once
#ifndef SURFACE_HXX
#    define SURFACE_HXX
#    include <ObjectiveVulkan/common/globals.hxx>

namespace Engine
{
    struct surface::OBJECTIVE_VULKAN_DATA_TYPE
    {
        OBJECTIVE_VULKAN_DATA_TYPE( types::surface parent, instance *instance, uint32_t width, uint32_t height );
        ~OBJECTIVE_VULKAN_DATA_TYPE();
        uint32_t width;
        uint32_t height;
        std::set<Engine::types::swapchain> swapchains;
        Engine::instance *instance;
        types::surface parent;
    };
} // namespace Engine
#endif