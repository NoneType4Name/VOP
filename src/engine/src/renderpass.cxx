#include <renderpass.hxx>
#include <swapchain.hxx>
#include <device.hxx>
#include <EHI.hxx>

namespace Engine
{
    void InstanceSetup::renderpassInfo( types::pass renderpass, types::link link, VkRenderPassCreateInfo &createInfo, std::vector<void *> &dataPointer, void *userPointer )
    {
        dataPointer.reserve( 4 );
        std::vector<VkAttachmentDescription> *attachments = static_cast<std::vector<VkAttachmentDescription> *>( dataPointer.emplace_back( new std::vector<VkAttachmentDescription>( 3 ) ) );

        VkAttachmentDescription *ColorAttachment { &( *attachments )[ 0 ] };
        ColorAttachment->format         = link->data->format.format;
        ColorAttachment->samples        = VK_SAMPLE_COUNT_2_BIT;
        ColorAttachment->loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
        ColorAttachment->storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
        ColorAttachment->stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        ColorAttachment->stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        ColorAttachment->initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
        ColorAttachment->finalLayout    = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentDescription *DepthAttachment { &( *attachments )[ 1 ] };
        DepthAttachment->format         = link->data->depthImageFormat;
        DepthAttachment->samples        = ColorAttachment->samples;
        DepthAttachment->loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
        DepthAttachment->storeOp        = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        DepthAttachment->stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        DepthAttachment->stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        DepthAttachment->initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
        DepthAttachment->finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentDescription *ColorAttachmentResolve { &( *attachments )[ 2 ] };
        ColorAttachmentResolve->format         = ColorAttachment->format;
        ColorAttachmentResolve->samples        = VK_SAMPLE_COUNT_1_BIT;
        ColorAttachmentResolve->loadOp         = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        ColorAttachmentResolve->storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
        ColorAttachmentResolve->stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        ColorAttachmentResolve->stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        ColorAttachmentResolve->initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
        ColorAttachmentResolve->finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference *ColorAttachmentRef { static_cast<VkAttachmentReference *>( dataPointer.emplace_back( new VkAttachmentReference {} ) ) };
        ColorAttachmentRef->attachment = 0;
        ColorAttachmentRef->layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentReference *DepthAttachmentRef { static_cast<VkAttachmentReference *>( dataPointer.emplace_back( new VkAttachmentReference {} ) ) };
        DepthAttachmentRef->attachment = 1;
        DepthAttachmentRef->layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference *ColorAttachmentResolveRef { static_cast<VkAttachmentReference *>( dataPointer.emplace_back( new VkAttachmentReference {} ) ) };
        ColorAttachmentResolveRef->attachment = 2;
        ColorAttachmentResolveRef->layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription *subpass { static_cast<VkSubpassDescription *>( dataPointer.emplace_back( new VkSubpassDescription {} ) ) };
        subpass->pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass->colorAttachmentCount    = 1;
        subpass->pColorAttachments       = ColorAttachmentRef;
        subpass->pDepthStencilAttachment = DepthAttachmentRef;
        subpass->pResolveAttachments     = ColorAttachmentResolveRef;

        VkSubpassDependency *dependency { static_cast<VkSubpassDependency *>( dataPointer.emplace_back( new VkSubpassDescription {} ) ) };
        dependency->srcSubpass    = VK_SUBPASS_EXTERNAL;
        dependency->dstSubpass    = 0;
        dependency->srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency->srcAccessMask = 0;
        dependency->dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency->dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        createInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        createInfo.attachmentCount = attachments->size();
        createInfo.pAttachments    = attachments->data();
        createInfo.subpassCount    = 1;
        createInfo.pSubpasses      = subpass;
        createInfo.dependencyCount = 1;
        createInfo.pDependencies   = dependency;
    }

    void InstanceSetup::renderpassInfoClear( types::pass renderpass, types::link link, std::vector<void *> &dataPointer, void *userPointer )
    {
        static_cast<std::vector<VkAttachmentDescription> *>( dataPointer[ 0 ] )->clear();
        delete static_cast<VkAttachmentReference *>( dataPointer[ 1 ] );
        delete static_cast<VkAttachmentReference *>( dataPointer[ 2 ] );
        delete static_cast<VkAttachmentReference *>( dataPointer[ 3 ] );
        delete static_cast<VkSubpassDescription *>( dataPointer[ 4 ] );
        delete static_cast<VkSubpassDependency *>( dataPointer[ 5 ] );
        dataPointer.clear();
    }

    pass::pass() = default;

    pass::pass( types::link link )
    {
        DEFINE_DATA_FIELD
        data->link = link;
        std::vector<void *> pData;
        VkRenderPassCreateInfo createInfo {};
        data->link->data->window->data->instance->data->setup->renderpassInfo( this, data->link, createInfo, pData, data->link->data->window->data->instance->data->userPointer );
        createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        CHECK_RESULT( vkCreateRenderPass( link->data->device->data->device, &createInfo, ALLOCATION_CALLBACK, &data->handle ) );
        data->link->data->window->data->instance->data->setup->renderpassInfoClear( this, data->link, pData, data->link->data->window->data->instance->data->userPointer );
    }

    pass::~pass()
    {
        vkDestroyRenderPass( data->link->data->device->data->device, data->handle, ALLOCATION_CALLBACK );
    }
} // namespace Engine