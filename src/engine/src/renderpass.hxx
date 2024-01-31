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
        DATA_TYPE( types::renderPass parent, types::device device );
        ~DATA_TYPE();
        void create( VkRenderPassCreateInfo createInfo );
        types::device device;
        types::renderPass parent;

      private:
        friend renderPass;
        struct _attachment
        {
            VkFormat format;
            VkSampleCountFlagBits samples;
        };
        std::vector<_attachment> attachments;
        std::vector<uint32_t> attachmentsRefs;
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