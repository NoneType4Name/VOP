#pragma once
#ifndef RENDERPASS_HXX
#    define RENDERPASS_HXX
#    include <common/globals.hxx>

namespace Engine
{
    struct renderPass::DATA_TYPE
    {
        DATA_TYPE( types::renderPass parent, types::device device );
        ~DATA_TYPE();
        void create( VkRenderPassCreateInfo createInfo );
        std::set<types::framebuffer> framebuffers;
        types::device device;
        types::renderPass parent;

      private:
        friend renderPass;
        struct _attachment
        {
            VkFormat format;
            VkSampleCountFlagBits samples;
        };
        struct _colorAttachmentRef
        {
            uint32_t ColorAttachment;
            uint32_t ResolveAttachment;
            uint32_t DepthStencilAttachment;
        };
        struct _subpassDescription
        {
            VkSubpassDescriptionFlags flags;
            VkPipelineBindPoint pipelineBindPoint;
            std::vector<uint32_t> inputAttachmentRefs;
            std::vector<_colorAttachmentRef> colorAttachmentRefs;
            std::vector<uint32_t> preserveAttachmentRefs;
        };
        std::vector<_attachment> attachments;
        std::vector<_subpassDescription> subpasses;
        std::vector<VkSubpassDependency> dependencies;
    };

    struct framebuffer::DATA_TYPE
    {
        DATA_TYPE( types::framebuffer parent, types::renderPass renderPass );
        ~DATA_TYPE();
        void create( VkFramebufferCreateInfo createInfo, std::vector<types::image> attachments );
        types::renderPass renderpass;
        types::framebuffer parent;
    };
} // namespace Engine
#endif