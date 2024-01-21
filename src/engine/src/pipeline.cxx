// #include <RHI.hxx>
#include <device.hxx>
// #include <model.hxx>
#include <pipeline.hxx>
#include <renderpass.hxx>

namespace Engine
{
    layout::layout( types::device device, types::descriptorPool pool, void *userData )
    {
        DEFINE_DATA_FIELD
        data->device = device;
        data->pool   = pool;
        VkPipelineLayoutCreateInfo ci {};
        ci.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        ci.setLayoutCount         = data->pool->data->layouts.size();
        ci.pSetLayouts            = data->pool->data->layouts.data();
        ci.pushConstantRangeCount = 0;
        ci.pPushConstantRanges    = 0;
        vkCreatePipelineLayout( data->device->handle, &ci, ALLOCATION_CALLBACK, &data->handle );
    }
    layout::~layout()
    {
        vkDestroyPipelineLayout( data->device->handle, data->handle, ALLOCATION_CALLBACK );
    }

    void InstanceSetup::layoutInfo( types::layout layout, const void *&pNext, VkPipelineLayoutCreateFlags &flags, std::vector<void *> &dataPointer, void *userPoiner ) {}
    void InstanceSetup::layoutInfoClear( types::layout layout, std::vector<void *> &dataPointer, void *userPoiner ) {}

    void InstanceSetup::pipelineInfo( types::pipeline pipeline, VkGraphicsPipelineCreateInfo &createInfo, std::vector<void *> &dataPointer, void *userPoiner )
    {
        uint32_t index { 0 };
        dataPointer.resize( 13 );
        VkPipelineVertexInputStateCreateInfo *vertexInput;
        VkPipelineInputAssemblyStateCreateInfo *inputAssembly;
        VkPipelineViewportStateCreateInfo *viewport;
        VkPipelineRasterizationStateCreateInfo *rasterization;
        VkPipelineMultisampleStateCreateInfo *multisampling;
        VkPipelineDepthStencilStateCreateInfo *depthStencil;
        VkPipelineColorBlendAttachmentState *colorBlendAttachment;
        VkPipelineColorBlendStateCreateInfo *colorBlend;
        VkPipelineDynamicStateCreateInfo *dynamicState;
        createInfo.pVertexInputState = vertexInput   = static_cast<VkPipelineVertexInputStateCreateInfo *>( dataPointer[ index++ ] = new VkPipelineVertexInputStateCreateInfo {} );
        vertexInput->vertexBindingDescriptionCount   = 1;
        vertexInput->pVertexBindingDescriptions      = static_cast<VkVertexInputBindingDescription *>( dataPointer[ index++ ] = new VkVertexInputBindingDescription { 0, sizeof( index ), VK_VERTEX_INPUT_RATE_VERTEX } );
        vertexInput->vertexAttributeDescriptionCount = 3;
        vertexInput->pVertexAttributeDescriptions    = static_cast<std::vector<VkVertexInputAttributeDescription> *>( dataPointer[ index++ ] = new std::vector<VkVertexInputAttributeDescription> {
                                                                                                                       { 0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0 },
                                                                                                                       { 1, 0, VK_FORMAT_R32G32B32A32_SFLOAT, sizeof( index ) },
                                                                                                                       { 2, 0, VK_FORMAT_R32G32_SFLOAT, sizeof( index ) * 2 } } )
                                                        ->data();

        createInfo.pInputAssemblyState = inputAssembly = static_cast<VkPipelineInputAssemblyStateCreateInfo *>( dataPointer[ index++ ] = new VkPipelineInputAssemblyStateCreateInfo {} ); // 5
        inputAssembly->primitiveRestartEnable          = VK_FALSE;
        inputAssembly->topology                        = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

        createInfo.pViewportState = viewport = static_cast<VkPipelineViewportStateCreateInfo *>( dataPointer[ index++ ] = new VkVertexInputBindingDescription {} ); // 6
        viewport->viewportCount              = 1;
        viewport->scissorCount               = 1;

        createInfo.pRasterizationState = rasterization = static_cast<VkPipelineRasterizationStateCreateInfo *>( dataPointer[ index++ ] = new VkPipelineRasterizationStateCreateInfo {} ); // 7
        rasterization->depthClampEnable                = VK_FALSE;
        rasterization->rasterizerDiscardEnable         = VK_FALSE;
        rasterization->polygonMode                     = VK_POLYGON_MODE_FILL;
        rasterization->lineWidth                       = 1.f;
        rasterization->cullMode                        = VK_CULL_MODE_NONE;
        rasterization->frontFace                       = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterization->depthBiasEnable                 = VK_FALSE;

        createInfo.pMultisampleState = multisampling = static_cast<VkPipelineMultisampleStateCreateInfo *>( dataPointer[ index++ ] = new VkPipelineMultisampleStateCreateInfo {} ); // 8
        multisampling->sampleShadingEnable           = VK_FALSE;
        multisampling->rasterizationSamples          = VK_SAMPLE_COUNT_2_BIT;
        multisampling->sampleShadingEnable           = VK_TRUE;
        multisampling->minSampleShading              = .4f;

        createInfo.pDepthStencilState = depthStencil = static_cast<VkPipelineDepthStencilStateCreateInfo *>( dataPointer[ index++ ] = new VkPipelineDepthStencilStateCreateInfo {} ); // 9
        depthStencil->depthTestEnable                = VK_TRUE;
        depthStencil->depthWriteEnable               = VK_TRUE;
        depthStencil->depthCompareOp                 = VK_COMPARE_OP_LESS;
        depthStencil->depthBoundsTestEnable          = VK_FALSE;
        depthStencil->minDepthBounds                 = 0.f;
        depthStencil->maxDepthBounds                 = 1.f;
        depthStencil->stencilTestEnable              = VK_FALSE;

        createInfo.pColorBlendState = colorBlend = static_cast<VkPipelineColorBlendStateCreateInfo *>( dataPointer[ index++ ] = new VkPipelineColorBlendStateCreateInfo {} ); // 10
        colorBlend->logicOpEnable                = VK_FALSE;
        colorBlend->attachmentCount              = 1;

        colorBlend->pAttachments = colorBlendAttachment = static_cast<VkPipelineColorBlendAttachmentState *>( dataPointer[ index++ ] = new VkPipelineColorBlendAttachmentState {} ); // 11
        colorBlendAttachment->colorWriteMask            = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment->blendEnable               = VK_FALSE;
        colorBlendAttachment->srcColorBlendFactor       = VK_BLEND_FACTOR_ONE;  // Optional
        colorBlendAttachment->dstColorBlendFactor       = VK_BLEND_FACTOR_ZERO; // Optional
        colorBlendAttachment->colorBlendOp              = VK_BLEND_OP_ADD;      // Optional
        colorBlendAttachment->srcAlphaBlendFactor       = VK_BLEND_FACTOR_ONE;  // Optional
        colorBlendAttachment->dstAlphaBlendFactor       = VK_BLEND_FACTOR_ZERO; // Optional
        colorBlendAttachment->alphaBlendOp              = VK_BLEND_OP_ADD;      // Optional

        createInfo.pDynamicState = dynamicState = static_cast<VkPipelineDynamicStateCreateInfo *>( dataPointer[ index++ ] = new VkPipelineDynamicStateCreateInfo {} ); // 12
        dynamicState->dynamicStateCount         = 2;
        dynamicState->pDynamicStates            = ( static_cast<std::vector<VkDynamicState> *>( dataPointer[ index++ ] = new std::vector<VkDynamicState> { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR } ) )->data();

        createInfo.subpass = 0;
    }

    void InstanceSetup::pipelineInfoClear( types::pipeline pipeline, std::vector<void *> &dataPointer, void *userPoiner )
    {
        uint32_t index { 0 };
        delete static_cast<VkPipelineVertexInputStateCreateInfo *>( dataPointer[ index++ ] );
        delete static_cast<VkVertexInputBindingDescription *>( dataPointer[ index++ ] );
        static_cast<std::vector<VkVertexInputAttributeDescription> *>( dataPointer[ index++ ] )->clear();
        delete static_cast<VkVertexInputAttributeDescription *>( dataPointer[ index++ ] );
        delete static_cast<VkVertexInputAttributeDescription *>( dataPointer[ index++ ] );
        delete static_cast<VkPipelineInputAssemblyStateCreateInfo *>( dataPointer[ index++ ] );
        delete static_cast<VkPipelineViewportStateCreateInfo *>( dataPointer[ index++ ] );
        delete static_cast<VkPipelineRasterizationStateCreateInfo *>( dataPointer[ index++ ] );
        delete static_cast<VkPipelineMultisampleStateCreateInfo *>( dataPointer[ index++ ] );
        delete static_cast<VkPipelineDepthStencilStateCreateInfo *>( dataPointer[ index++ ] );
        delete static_cast<VkPipelineColorBlendStateCreateInfo *>( dataPointer[ index++ ] );
        delete static_cast<VkPipelineColorBlendAttachmentState *>( dataPointer[ index++ ] );
        delete static_cast<VkPipelineDynamicStateCreateInfo *>( dataPointer[ index++ ] );
        static_cast<std::vector<VkDynamicState> *>( dataPointer[ index++ ] )->clear();
    }

    pipeline::pipeline() {}
    pipeline::pipeline( types::device device, types::layout layout, std::vector<types::shader> shaders, types::pass pass )
    {
        DEFINE_DATA_FIELD
        data->device     = device;
        data->layout     = layout;
        data->renderpass = pass;
        std::vector<void *> dataPointer;
        std::vector<VkPipelineShaderStageCreateInfo> ShaderStages;
        ShaderStages.reserve( shaders.size() );
        for ( auto &shader : shaders )
            ShaderStages.push_back( { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, 0, 0, shader->data->stage, shader->data->module, shader->data->pName, 0 } );

        VkGraphicsPipelineCreateInfo GraphicPipeLineCreateInfo {};
        data->device->data->window->data->instance->data->setup->pipelineInfo( this, GraphicPipeLineCreateInfo, dataPointer, data->device->data->window->data->instance->data->userPointer );
        GraphicPipeLineCreateInfo.sType      = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        GraphicPipeLineCreateInfo.stageCount = ShaderStages.size();
        GraphicPipeLineCreateInfo.pStages    = ShaderStages.data();
        GraphicPipeLineCreateInfo.layout     = data->layout->data->handle;
        GraphicPipeLineCreateInfo.renderPass = data->renderpass->data->handle;

        if ( GraphicPipeLineCreateInfo.pVertexInputState )
            const_cast<VkPipelineVertexInputStateCreateInfo *>( GraphicPipeLineCreateInfo.pVertexInputState )->sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        if ( GraphicPipeLineCreateInfo.pInputAssemblyState )
            const_cast<VkPipelineInputAssemblyStateCreateInfo *>( GraphicPipeLineCreateInfo.pInputAssemblyState )->sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        if ( GraphicPipeLineCreateInfo.pTessellationState )
            const_cast<VkPipelineTessellationStateCreateInfo *>( GraphicPipeLineCreateInfo.pTessellationState )->sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
        if ( GraphicPipeLineCreateInfo.pViewportState )
            const_cast<VkPipelineViewportStateCreateInfo *>( GraphicPipeLineCreateInfo.pViewportState )->sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        if ( GraphicPipeLineCreateInfo.pRasterizationState )
            const_cast<VkPipelineRasterizationStateCreateInfo *>( GraphicPipeLineCreateInfo.pRasterizationState )->sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        if ( GraphicPipeLineCreateInfo.pMultisampleState )
            const_cast<VkPipelineMultisampleStateCreateInfo *>( GraphicPipeLineCreateInfo.pMultisampleState )->sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        if ( GraphicPipeLineCreateInfo.pDepthStencilState )
            const_cast<VkPipelineDepthStencilStateCreateInfo *>( GraphicPipeLineCreateInfo.pDepthStencilState )->sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        if ( GraphicPipeLineCreateInfo.pColorBlendState )
            const_cast<VkPipelineColorBlendStateCreateInfo *>( GraphicPipeLineCreateInfo.pColorBlendState )->sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        if ( GraphicPipeLineCreateInfo.pDynamicState )
            const_cast<VkPipelineDynamicStateCreateInfo *>( GraphicPipeLineCreateInfo.pDynamicState )->sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        CHECK_RESULT( vkCreateGraphicsPipelines( data->device->handle, nullptr, 1, &GraphicPipeLineCreateInfo, ALLOCATION_CALLBACK, &data->handle ) ); // TODO pipeline cachae
        data->device->data->window->data->instance->data->setup->pipelineInfoClear( this, dataPointer, data->device->data->window->data->instance->data->userPointer );
    }

    pipeline::~pipeline()
    {
        vkDestroyPipeline( data->device->handle, data->handle, ALLOCATION_CALLBACK );
        vkDestroyPipelineLayout( data->device->handle, data->layout->data->handle, ALLOCATION_CALLBACK );
    }
} // namespace Engine
