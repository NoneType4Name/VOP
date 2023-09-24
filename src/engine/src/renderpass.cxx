#include <renderpass.hxx>
#include <swapchain.hxx>
#include <device.hxx>
#include <RHI.hxx>

namespace Engine
{
    namespace tools
    {
        namespace
        {
            VkRenderPass _renderpass{ nullptr };
        }

        void createRenderPass()
        {
            VkAttachmentDescription ColorAttachment{};
            ColorAttachment.format         = tools::getSwapchainSurfaceFormat().format;
            ColorAttachment.samples        = static_cast<VkSampleCountFlagBits>( tools::getSettings().settings.MultiSamplingCount );
            ColorAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
            ColorAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
            ColorAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            ColorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            ColorAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
            ColorAttachment.finalLayout    = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            VkAttachmentDescription ColorAttachmentResolve{};
            ColorAttachmentResolve.format         = tools::getSwapchainSurfaceFormat().format;
            ColorAttachmentResolve.samples        = VK_SAMPLE_COUNT_1_BIT;
            ColorAttachmentResolve.loadOp         = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            ColorAttachmentResolve.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
            ColorAttachmentResolve.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            ColorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            ColorAttachmentResolve.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
            ColorAttachmentResolve.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

            VkAttachmentDescription DepthAttachment{};
            DepthAttachment.format         = tools::getSwapchainDepthImageFormat();
            DepthAttachment.samples        = ColorAttachment.samples;
            DepthAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
            DepthAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            DepthAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            DepthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            DepthAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
            DepthAttachment.finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

            VkAttachmentReference ColorAttachmentRef{};
            ColorAttachmentRef.attachment = 0;
            ColorAttachmentRef.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            VkAttachmentReference DepthAttachmentRef{};
            DepthAttachmentRef.attachment = 1;
            DepthAttachmentRef.layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

            VkAttachmentReference ColorAttachmentResolveRef{};
            ColorAttachmentResolveRef.attachment = 2;
            ColorAttachmentResolveRef.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            VkSubpassDescription subpass{};
            subpass.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpass.colorAttachmentCount    = 1;
            subpass.pColorAttachments       = &ColorAttachmentRef;
            subpass.pResolveAttachments     = &ColorAttachmentResolveRef;
            subpass.pDepthStencilAttachment = &DepthAttachmentRef;

            VkSubpassDependency dependency{};
            dependency.srcSubpass    = VK_SUBPASS_EXTERNAL;
            dependency.dstSubpass    = 0;
            dependency.srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            dependency.srcAccessMask = 0;
            dependency.dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

            VkAttachmentDescription Attachments[]{ ColorAttachment, DepthAttachment, ColorAttachmentResolve };

            VkRenderPassCreateInfo renderPassInfo{};
            renderPassInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
            renderPassInfo.attachmentCount = sizeof( Attachments ) / sizeof( Attachments[ 0 ] );
            renderPassInfo.pAttachments    = Attachments;
            renderPassInfo.subpassCount    = 1;
            renderPassInfo.pSubpasses      = &subpass;
            renderPassInfo.dependencyCount = 1;
            renderPassInfo.pDependencies   = &dependency;
            CHECK_RESULT( vkCreateRenderPass( tools::getDevice(), &renderPassInfo, nullptr, &_renderpass ) );
        }

        VkRenderPass getRenderPass()
        {
            return _renderpass;
        }

        void destroyRenderPass()
        {
            vkDestroyRenderPass( tools::getDevice(), _renderpass, ALLOCATION_CALLBACK );
        }
    } // namespace tools
} // namespace Engine