#include <renderpass.hxx>
#include <swapchain.hxx>
#include <device.hxx>
#include <instance.hxx>
#include <common/logging.hxx>

namespace Engine
{
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
        vkDestroyRenderPass( data->device->handle, handle, ENGINE_ALLOCATION_CALLBACK );
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
        CHECK_RESULT( vkCreateRenderPass( device->handle, &createInfo, ENGINE_ALLOCATION_CALLBACK, &parent->handle ) );
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
        vkDestroyFramebuffer( data->renderpass->data->device->handle, handle, ENGINE_ALLOCATION_CALLBACK );
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
        CHECK_RESULT( vkCreateFramebuffer( renderpass->data->device->handle, &createInfo, ENGINE_ALLOCATION_CALLBACK, &parent->handle ) );
    }
} // namespace Engine