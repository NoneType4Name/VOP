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
        VkRenderPass handle { nullptr };
        Engine::types::link link { nullptr };
    };
} // namespace Engine
#endif