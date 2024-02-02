#include <renderpass.hxx>
#include <swapchain.hxx>
#include <device.hxx>
#include <instance.hxx>

namespace Engine
{
    // void InstanceSetup::renderpassInfo( types::pass renderpass, types::swapchain swapchain, VkRenderPassCreateInfo &createInfo, std::vector<void *> &dataPointer, void *userPointer )
    // {
    //     dataPointer.reserve( 4 );
    //     std::vector<VkAttachmentDescription> *attachments = static_cast<std::vector<VkAttachmentDescription> *>( dataPointer.emplace_back( new std::vector<VkAttachmentDescription>( 3 ) ) );

    //     VkAttachmentDescription *ColorAttachment { &( *attachments )[ 0 ] };
    //     ColorAttachment->format         = swapchain->data->format.format;
    //     ColorAttachment->samples        = VK_SAMPLE_COUNT_2_BIT;
    //     ColorAttachment->loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
    //     ColorAttachment->storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
    //     ColorAttachment->stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    //     ColorAttachment->stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    //     ColorAttachment->initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
    //     ColorAttachment->finalLayout    = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    //     VkAttachmentDescription *DepthAttachment { &( *attachments )[ 1 ] };
    //     DepthAttachment->format         = swapchain->data->depthImageFormat;
    //     DepthAttachment->samples        = ColorAttachment->samples;
    //     DepthAttachment->loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
    //     DepthAttachment->storeOp        = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    //     DepthAttachment->stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    //     DepthAttachment->stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    //     DepthAttachment->initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
    //     DepthAttachment->finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    //     VkAttachmentDescription *ColorAttachmentResolve { &( *attachments )[ 2 ] };
    //     ColorAttachmentResolve->format         = ColorAttachment->format;
    //     ColorAttachmentResolve->samples        = VK_SAMPLE_COUNT_1_BIT;
    //     ColorAttachmentResolve->loadOp         = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    //     ColorAttachmentResolve->storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
    //     ColorAttachmentResolve->stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    //     ColorAttachmentResolve->stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    //     ColorAttachmentResolve->initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
    //     ColorAttachmentResolve->finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    //     VkAttachmentReference *ColorAttachmentRef { static_cast<VkAttachmentReference *>( dataPointer.emplace_back( new VkAttachmentReference {} ) ) };
    //     ColorAttachmentRef->attachment = 0;
    //     ColorAttachmentRef->layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    //     VkAttachmentReference *DepthAttachmentRef { static_cast<VkAttachmentReference *>( dataPointer.emplace_back( new VkAttachmentReference {} ) ) };
    //     DepthAttachmentRef->attachment = 1;
    //     DepthAttachmentRef->layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    //     VkAttachmentReference *ColorAttachmentResolveRef { static_cast<VkAttachmentReference *>( dataPointer.emplace_back( new VkAttachmentReference {} ) ) };
    //     ColorAttachmentResolveRef->attachment = 2;
    //     ColorAttachmentResolveRef->layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    //     VkSubpassDescription *subpass { static_cast<VkSubpassDescription *>( dataPointer.emplace_back( new VkSubpassDescription {} ) ) };
    //     subpass->pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
    //     subpass->colorAttachmentCount    = 1;
    //     subpass->pColorAttachments       = ColorAttachmentRef;
    //     subpass->pDepthStencilAttachment = DepthAttachmentRef;
    //     subpass->pResolveAttachments     = ColorAttachmentResolveRef;

    //     VkSubpassDependency *dependency { static_cast<VkSubpassDependency *>( dataPointer.emplace_back( new VkSubpassDescription {} ) ) };
    //     dependency->srcSubpass    = VK_SUBPASS_EXTERNAL;
    //     dependency->dstSubpass    = 0;
    //     dependency->srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    //     dependency->srcAccessMask = 0;
    //     dependency->dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    //     dependency->dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    //     createInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    //     createInfo.attachmentCount = attachments->size();
    //     createInfo.pAttachments    = attachments->data();
    //     createInfo.subpassCount    = 1;
    //     createInfo.pSubpasses      = subpass;
    //     createInfo.dependencyCount = 1;
    //     createInfo.pDependencies   = dependency;
    // }

    // void InstanceSetup::renderpassInfoClear( types::pass renderpass, types::swapchain swapchain, std::vector<void *> &dataPointer, void *userPointer )
    // {
    //     static_cast<std::vector<VkAttachmentDescription> *>( dataPointer[ 0 ] )->clear();
    //     delete static_cast<VkAttachmentReference *>( dataPointer[ 1 ] );
    //     delete static_cast<VkAttachmentReference *>( dataPointer[ 2 ] );
    //     delete static_cast<VkAttachmentReference *>( dataPointer[ 3 ] );
    //     delete static_cast<VkSubpassDescription *>( dataPointer[ 4 ] );
    //     delete static_cast<VkSubpassDependency *>( dataPointer[ 5 ] );
    //     dataPointer.clear();
    // }

    renderPass::renderPass( bool, types::device device, VkRenderPassCreateInfo createInfo )
    {
        DEFINE_DATA_FIELD( device );
        data->device->data->renderpasses.insert( this );
    }

    renderPass::renderPass( types::device device, VkRenderPassCreateInfo createInfo ) :
        renderPass( 1, device, createInfo )
    {
        data->create( createInfo );
    }

    renderPass::~renderPass()
    {
        auto frm { data->framebuffers.begin() };
        while ( frm != data->framebuffers.end() )
            delete *frm++;
        vkDestroyRenderPass( data->device->handle, handle, ALLOCATION_CALLBACK );
        data->device->data->renderpasses.erase( this );
    }

    bool renderPass::compatible( types::renderPass right )
    {
        if ( data->subpasses.size() - right->data->subpasses.size() )
            return false;
        for ( uint32_t subpass { 0 }; subpass < data->subpasses.size(); ++subpass )
        {
            if ( data->subpasses[ subpass ].flags != right->data->subpasses[ subpass ].flags )
                return false;
            if ( data->subpasses[ subpass ].pipelineBindPoint != right->data->subpasses[ subpass ].pipelineBindPoint )
                return false;
            auto m { data->subpasses[ subpass ].colorAttachmentRefs.size() };
            if ( m < right->data->subpasses[ subpass ].colorAttachmentRefs.size() )
                m = right->data->subpasses[ subpass ].colorAttachmentRefs.size();
            std::vector<DATA_TYPE::_colorAttachmentRef> colorRefs { data->subpasses[ subpass ].colorAttachmentRefs.begin(), data->subpasses[ subpass ].colorAttachmentRefs.end() };
            std::vector<DATA_TYPE::_colorAttachmentRef> colorRefs2 { right->data->subpasses[ subpass ].colorAttachmentRefs.begin(), right->data->subpasses[ subpass ].colorAttachmentRefs.end() };
            colorRefs.resize( m, { VK_ATTACHMENT_UNUSED, VK_ATTACHMENT_UNUSED, VK_ATTACHMENT_UNUSED } );
            colorRefs2.resize( m, { VK_ATTACHMENT_UNUSED, VK_ATTACHMENT_UNUSED, VK_ATTACHMENT_UNUSED } );
            for ( uint32_t colorRef { 0 }; colorRef < m; ++colorRef )
            {
                if ( colorRefs[ colorRef ].ColorAttachment != VK_ATTACHMENT_UNUSED && colorRefs2[ colorRef ].ColorAttachment != VK_ATTACHMENT_UNUSED )
                {
                    if ( data->attachments[ colorRefs[ colorRef ].ColorAttachment ].format != right->data->attachments[ colorRefs2[ colorRef ].ColorAttachment ].format ||
                         data->attachments[ colorRefs[ colorRef ].ColorAttachment ].samples != right->data->attachments[ colorRefs2[ colorRef ].ColorAttachment ].samples )
                        return false;
                    if ( !( data->subpasses.size() - 1 ) )
                        break;
                    if ( data->attachments[ colorRefs[ colorRef ].DepthStencilAttachment ].format != right->data->attachments[ colorRefs2[ colorRef ].DepthStencilAttachment ].format ||
                         data->attachments[ colorRefs[ colorRef ].DepthStencilAttachment ].samples != right->data->attachments[ colorRefs2[ colorRef ].DepthStencilAttachment ].samples )
                        return false;
                    if ( data->attachments[ colorRefs[ colorRef ].ResolveAttachment ].format != right->data->attachments[ colorRefs2[ colorRef ].ResolveAttachment ].format ||
                         data->attachments[ colorRefs[ colorRef ].ResolveAttachment ].samples != right->data->attachments[ colorRefs2[ colorRef ].ResolveAttachment ].samples )
                        return false;
                }
                else if ( colorRefs[ colorRef ].ColorAttachment != colorRefs2[ colorRef ].ColorAttachment )
                    return false;
            }
            m = data->subpasses[ subpass ].inputAttachmentRefs.size();
            if ( m < right->data->subpasses[ subpass ].inputAttachmentRefs.size() )
                m = right->data->subpasses[ subpass ].inputAttachmentRefs.size();
            std::vector<uint32_t> refs { data->subpasses[ subpass ].inputAttachmentRefs.begin(), data->subpasses[ subpass ].inputAttachmentRefs.end() };
            std::vector<uint32_t> refs2 { right->data->subpasses[ subpass ].inputAttachmentRefs.begin(), right->data->subpasses[ subpass ].inputAttachmentRefs.end() };
            refs.resize( m, VK_ATTACHMENT_UNUSED );
            refs2.resize( m, VK_ATTACHMENT_UNUSED );
            for ( uint32_t inputRef { 0 }; inputRef < m; ++inputRef )
            {
                if ( refs[ inputRef ] != VK_ATTACHMENT_UNUSED && refs2[ inputRef ] != VK_ATTACHMENT_UNUSED )
                {
                    if ( data->attachments[ refs[ inputRef ] ].format != right->data->attachments[ refs2[ inputRef ] ].format ||
                         data->attachments[ refs[ inputRef ] ].samples != right->data->attachments[ refs2[ inputRef ] ].samples )
                        return false;
                }
                else if ( refs[ inputRef ] != refs2[ inputRef ] )
                    return false;
            }
            if ( m < right->data->subpasses[ subpass ].preserveAttachmentRefs.size() )
                m = right->data->subpasses[ subpass ].preserveAttachmentRefs.size();
            refs.assign( data->subpasses[ subpass ].preserveAttachmentRefs.begin(), data->subpasses[ subpass ].preserveAttachmentRefs.end() );
            refs2.assign( right->data->subpasses[ subpass ].preserveAttachmentRefs.begin(), right->data->subpasses[ subpass ].preserveAttachmentRefs.end() );
            refs.resize( m, VK_ATTACHMENT_UNUSED );
            refs2.resize( m, VK_ATTACHMENT_UNUSED );
            for ( uint32_t preserveRef { 0 }; preserveRef < m; ++preserveRef )
            {
                if ( refs[ preserveRef ] != VK_ATTACHMENT_UNUSED && refs2[ preserveRef ] != VK_ATTACHMENT_UNUSED )
                {
                    if ( data->attachments[ refs[ preserveRef ] ].format != right->data->attachments[ refs2[ preserveRef ] ].format ||
                         data->attachments[ refs[ preserveRef ] ].samples != right->data->attachments[ refs2[ preserveRef ] ].samples )
                        return false;
                }
                else if ( refs[ preserveRef ] != refs2[ preserveRef ] )
                    return false;
            }
        }
        if ( data->dependencies.size() - right->data->dependencies.size() )
            return false;
        for ( uint32_t i { 0 }; i < data->dependencies.size(); ++i )
        {
            if ( data->dependencies[ i ].srcSubpass != right->data->dependencies[ i ].srcSubpass ||
                 data->dependencies[ i ].dstSubpass != right->data->dependencies[ i ].dstSubpass ||
                 data->dependencies[ i ].srcStageMask != right->data->dependencies[ i ].srcStageMask ||
                 data->dependencies[ i ].dstStageMask != right->data->dependencies[ i ].dstStageMask ||
                 data->dependencies[ i ].srcAccessMask != right->data->dependencies[ i ].srcAccessMask ||
                 data->dependencies[ i ].dstAccessMask != right->data->dependencies[ i ].dstAccessMask ||
                 data->dependencies[ i ].dependencyFlags != right->data->dependencies[ i ].dependencyFlags )
                return false;
        }
        return true;
    }

    renderPass::DATA_TYPE::DATA_TYPE( types::renderPass parent, types::device device ) :
        parent { parent }, device { device }
    {
    }

    renderPass::DATA_TYPE::~DATA_TYPE()
    {
    }

    void renderPass::DATA_TYPE::create( VkRenderPassCreateInfo createInfo )
    {
        createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        subpasses.resize( createInfo.subpassCount );
        dependencies.resize( createInfo.dependencyCount );
        attachments.resize( createInfo.attachmentCount );
        for ( uint32_t i { 0 }; i < createInfo.subpassCount; ++i )
        {
            const VkSubpassDescription *subpass { &createInfo.pSubpasses[ i ] };
            subpasses[ i ].flags             = subpass->flags;
            subpasses[ i ].pipelineBindPoint = subpass->pipelineBindPoint;
            subpasses[ i ].inputAttachmentRefs.resize( subpass->inputAttachmentCount );
            subpasses[ i ].colorAttachmentRefs.resize( subpass->colorAttachmentCount );
            subpasses[ i ].preserveAttachmentRefs.resize( subpass->preserveAttachmentCount );
            for ( uint32_t inputRef { 0 }; inputRef < subpass->inputAttachmentCount; ++inputRef )
                subpasses[ i ].inputAttachmentRefs[ inputRef ] = subpass->pInputAttachments[ inputRef ].attachment;
            for ( uint32_t colorRef { 0 }; colorRef < subpass->inputAttachmentCount; ++colorRef )
            {
                subpasses[ i ].colorAttachmentRefs[ colorRef ].ColorAttachment        = subpass->pColorAttachments[ colorRef ].attachment;
                subpasses[ i ].colorAttachmentRefs[ colorRef ].ResolveAttachment      = subpass->pResolveAttachments[ colorRef ].attachment;
                subpasses[ i ].colorAttachmentRefs[ colorRef ].DepthStencilAttachment = subpass->pDepthStencilAttachment[ colorRef ].attachment;
            }
        }
        for ( uint32_t i { 0 }; i < createInfo.dependencyCount; ++i )
            dependencies[ i ] = createInfo.pDependencies[ i ];
        for ( uint32_t i { 0 }; i < createInfo.attachmentCount; ++i )
        {
            attachments[ i ].format  = createInfo.pAttachments[ i ].format;
            attachments[ i ].samples = createInfo.pAttachments[ i ].samples;
        }
        CHECK_RESULT( vkCreateRenderPass( device->handle, &createInfo, ALLOCATION_CALLBACK, &parent->handle ) );
    }

    framebuffer::framebuffer( bool, types::renderPass renderPass, std::vector<types::image> attachments )
    {
        DEFINE_DATA_FIELD( renderPass );
        data->renderpass->data->framebuffers.insert( this );
    }

    framebuffer::framebuffer( types::renderPass renderPass, std::vector<types::image> attachments ) :
        framebuffer( 1, renderPass, attachments )
    {
        uint32_t w { ~0u }, h { ~0u }, l { ~0u };
        for ( const auto &atch : attachments )
        {
            if ( atch->properties.extent.width < w )
                w = atch->properties.extent.width;
            if ( atch->properties.extent.height < h )
                h = atch->properties.extent.height;
            if ( atch->properties.extent.depth < l )
                l = atch->properties.extent.depth;
        }
        VkFramebufferCreateInfo createInfo { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
        createInfo.width  = w;
        createInfo.height = h;
        createInfo.layers = l;
        data->create( createInfo, attachments );
    }

    framebuffer::~framebuffer()
    {
        vkDestroyFramebuffer( data->renderpass->data->device->handle, handle, ALLOCATION_CALLBACK );
        data->renderpass->data->framebuffers.erase( this );
    }

    framebuffer::DATA_TYPE::DATA_TYPE( types::framebuffer parent, types::renderPass renderpass ) :
        parent { parent }, renderpass { renderpass }
    {
    }

    framebuffer::DATA_TYPE::~DATA_TYPE()
    {
    }

    void framebuffer::DATA_TYPE::create( VkFramebufferCreateInfo createInfo, std::vector<types::image> attachments )
    {
        std::vector<VkImageView> attchms;
        attchms.reserve( attachments.size() );
        for ( const auto &atch : attachments )
            attchms.emplace_back( atch->view.handle );
        createInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        createInfo.renderPass      = renderpass->handle;
        createInfo.attachmentCount = attchms.size();
        createInfo.pAttachments    = attchms.data();
        CHECK_RESULT( vkCreateFramebuffer( renderpass->data->device->handle, &createInfo, ALLOCATION_CALLBACK, &parent->handle ) );
    }
} // namespace Engine