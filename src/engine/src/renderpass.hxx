#pragma once
#include <common/globals.hxx>
#include <common/logging.hxx>
#include <platform.hxx>

namespace Engine
{
    struct pass::DATA_TYPE
    {
        VkRenderPass handle { nullptr };
        Engine::types::link link { nullptr };
        virtual void setup( types::link link, VkRenderPassCreateInfo &createInfo, std::vector<void *> &dataPointer, void *userPointer );
    };
} // namespace Engine