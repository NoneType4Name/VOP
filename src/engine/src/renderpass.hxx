#pragma once
#include <common/globals.hxx>
#include <common/logging.hxx>
#include <platform.hxx>

namespace Engine
{
    namespace tools
    {
        void createRenderPass();
        void destroyRenderPass();
        VkRenderPass getRenderPass();
    } // namespace tools
} // namespace Engine