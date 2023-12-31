#pragma once
#ifndef RENDERPASS_HXX
#    define RENDERPASS_HXX
#    include <common/globals.hxx>
#    include <common/logging.hxx>
#    include <platform.hxx>

namespace Engine
{
    struct pass::DATA_TYPE
    {
        DATA_TYPE( types::pass parent ) :
            parent { parent } {}
        VkRenderPass handle { nullptr };
        Engine::types::swapchain swapchain { nullptr };
        types::pass parent;
    };
} // namespace Engine
#endif