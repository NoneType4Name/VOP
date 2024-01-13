#pragma once
#ifndef RENDERPASS_HXX
#    define RENDERPASS_HXX
#    include <common/globals.hxx>
#    include <common/logging.hxx>
#    include <platform.hxx>

namespace Engine
{
    struct renderPass::DATA_TYPE
    {
        DATA_TYPE( types::renderPass parent, types::swapchain swapchain );
        ~DATA_TYPE();
        void create( VkRenderPassCreateInfo createInfo );
        VkRenderPass handle { nullptr };
        types::swapchain swapchain;
        types::renderPass parent;
    };
} // namespace Engine
#endif