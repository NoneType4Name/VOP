#pragma once
#include <common/globals.hxx>
#include <common/logging.hxx>
#include <platform.hxx>

namespace Engine
{
    namespace render
    {
        struct subpass::DATA_TYPE
        {
            std::vector<VkAttachmentReference> referencies;
            std::vector<VkAttachmentDescription> attachments;
            Engine::types::link link { nullptr };
        };

        struct subpassLink::DATA_TYPE : VkSubpassDependency
        {
            std::vector<std::pair<subpass *, VkSubpassDependency>> subpasses;
        };

        struct pass::DATA_TYPE
        {
            VkRenderPass handle { nullptr };
            Engine::types::link link { nullptr };
            std::vector<VkAttachmentDescription> attachments;
        };
    } // namespace render
} // namespace Engine