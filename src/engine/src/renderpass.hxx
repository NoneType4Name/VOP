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
        void create( VkRenderPassCreateInfo createInfo, std::vector<VkAttachmentDescription> attachments, std::vector<VkSubpassDescription> subpasses, std::vector<VkSubpassDependency> subpassDependency );
        types::swapchain swapchain;
        types::renderPass parent;
    };

    struct framebuffer::DATA_TYPE
    {
        DATA_TYPE( types::framebuffer parent, types::renderPass renderPass, std::vector<types::image> attachments );
        ~DATA_TYPE();
        void create( VkFramebufferCreateInfo createInfo, types::renderPass renderPass, std::vector<types::image> attachments );
        types::framebuffer parent;
    };
} // namespace Engine
#endif