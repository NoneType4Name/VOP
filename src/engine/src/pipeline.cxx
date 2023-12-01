// #include <RHI.hxx>
#include <device.hxx>
// #include <model.hxx>
#include <pipeline.hxx>
#include <renderpass.hxx>

namespace Engine
{
    layout::layout( types::device device, types::descriptorPool pool, void *userData )
    {
        data->device = device;
        data->pool   = pool;
        VkPipelineLayoutCreateInfo ci {};
        ci.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        ci.setLayoutCount         = data->pool->data->layouts.size();
        ci.pSetLayouts            = data->pool->data->layouts.data();
        ci.pushConstantRangeCount = 0;
        ci.pPushConstantRanges    = 0;
        vkCreatePipelineLayout( data->device->data->device, &ci, ALLOCATION_CALLBACK, &data->handle );
    }
    layout::~layout()
    {
        vkDestroyPipelineLayout( data->device->data->device, data->handle, ALLOCATION_CALLBACK );
    }

    void InstanceSetup::layoutNextChain( types::layout layout, const void *&pNext, std::vector<std::unique_ptr<void>> &dataPointer, void *userPoiner )
    {
    }
    void InstanceSetup::layoutFlags( types::layout layout, VkPipelineLayoutCreateFlags &flags, void *userPoiner )
    {
    }

    void InstanceSetup::pipelineFlags( types::pipeline pipeline, VkPipelineCreateFlags &flags, void *userPoiner )
    {
    }

    void InstanceSetup::pipelineNextChain( types::pipeline pipeline, const void *&pNext, std::vector<std::unique_ptr<void>> &dataPointer, void *userPoiner )
    {
    }

    void InstanceSetup::pipelineVertexInputState( types::pipeline pipeline, VkPipelineVertexInputStateCreateInfo &stageInfo, std::vector<std::unique_ptr<void>> &dataPointer, void *userPoiner )
    {
        uint32_t vertex;
        dataPointer.resize( 4 );
        dataPointer[ 0 ].reset( new VkVertexInputBindingDescription { 0, sizeof( vertex ), VK_VERTEX_INPUT_RATE_VERTEX } );
        dataPointer[ 1 ].reset( new VkVertexInputAttributeDescription { 0, 0, VK_FORMAT_R32G32B32_SFLOAT, sizeof( vertex ) } );
        dataPointer[ 2 ]                          = new VkVertexInputAttributeDescription { 1, 0, VK_FORMAT_R32G32B32A32_SFLOAT, sizeof( vertex ) };
        dataPointer[ 3 ]                          = new VkVertexInputAttributeDescription { 2, 0, VK_FORMAT_R32G32_SFLOAT, sizeof( vertex ) };
        stageInfo.vertexBindingDescriptionCount   = 1;
        stageInfo.pVertexBindingDescriptions      = static_cast<VkVertexInputBindingDescription *>( dataPointer[ 0 ] );
        stageInfo.vertexAttributeDescriptionCount = 3;
        stageInfo.pVertexAttributeDescriptions    = static_cast<VkVertexInputAttributeDescription *>( dataPointer[ 1 ] );
    }

    void InstanceSetup::pipelineInputAssemblyState( types::pipeline pipeline, VkPipelineInputAssemblyStateCreateInfo &stageInfo, std::vector<std::unique_ptr<void>> &dataPointer, void *userPoiner )
    {
        stageInfo.primitiveRestartEnable = VK_FALSE;
        stageInfo.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    }

    void InstanceSetup::pipelineTessellationState( types::pipeline pipeline, VkPipelineTessellationStateCreateInfo &stageInfo, std::vector<std::unique_ptr<void>> &dataPointer, void *userPoiner )
    {
    }

    void InstanceSetup::pipelineViewportState( types::pipeline pipeline, VkPipelineViewportStateCreateInfo &stageInfo, std::vector<std::unique_ptr<void>> &dataPointer, void *userPoiner )
    {
        stageInfo.viewportCount = 1;
        stageInfo.scissorCount  = 1;
    }

    void InstanceSetup::pipelineRasterizationState( types::pipeline pipeline, VkPipelineRasterizationStateCreateInfo &stageInfo, std::vector<std::unique_ptr<void>> &dataPointer, void *userPoiner )
    {
        stageInfo.depthClampEnable        = VK_FALSE;
        stageInfo.rasterizerDiscardEnable = VK_FALSE;
        stageInfo.polygonMode             = VK_POLYGON_MODE_FILL;
        stageInfo.lineWidth               = 1.f;
        stageInfo.cullMode                = VK_CULL_MODE_NONE;
        stageInfo.frontFace               = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        stageInfo.depthBiasEnable         = VK_FALSE;
    }

    void InstanceSetup::pipelineMultisampleState( types::pipeline pipeline, VkPipelineMultisampleStateCreateInfo &stageInfo, std::vector<std::unique_ptr<void>> &dataPointer, void *userPoiner )
    {
        stageInfo.sampleShadingEnable  = VK_FALSE;
        stageInfo.rasterizationSamples = VK_SAMPLE_COUNT_2_BIT;
        stageInfo.sampleShadingEnable  = VK_TRUE;
        stageInfo.minSampleShading     = .4f;
    }

    void InstanceSetup::pipelineDepthStencilState( types::pipeline pipeline, VkPipelineDepthStencilStateCreateInfo &stageInfo, std::vector<std::unique_ptr<void>> &dataPointer, void *userPoiner )
    {
        stageInfo.depthTestEnable       = VK_TRUE;
        stageInfo.depthWriteEnable      = VK_TRUE;
        stageInfo.depthCompareOp        = VK_COMPARE_OP_LESS;
        stageInfo.depthBoundsTestEnable = VK_FALSE;
        stageInfo.minDepthBounds        = 0.f;
        stageInfo.maxDepthBounds        = 1.f;
        stageInfo.stencilTestEnable     = VK_FALSE;
    }

    void InstanceSetup::pipelineColorBlendState( types::pipeline pipeline, VkPipelineColorBlendStateCreateInfo &stageInfo, std::vector<std::unique_ptr<void>> &dataPointer, void *userPoiner )
    {
        dataPointer.resize( 1 );
        VkPipelineColorBlendAttachmentState *colorBlendAttachment { static_cast<VkPipelineColorBlendAttachmentState *>( dataPointer[ 0 ].get() ) };
        colorBlendAttachment->colorWriteMask      = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment->blendEnable         = VK_FALSE;
        colorBlendAttachment->srcColorBlendFactor = VK_BLEND_FACTOR_ONE;  // Optional
        colorBlendAttachment->dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        colorBlendAttachment->colorBlendOp        = VK_BLEND_OP_ADD;      // Optional
        colorBlendAttachment->srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;  // Optional
        colorBlendAttachment->dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        colorBlendAttachment->alphaBlendOp        = VK_BLEND_OP_ADD;      // Optional

        stageInfo.sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        stageInfo.logicOpEnable   = VK_FALSE;
        stageInfo.attachmentCount = 1;
        stageInfo.pAttachments    = colorBlendAttachment;
    }

    void InstanceSetup::pipelineDynamicState( types::pipeline pipeline, VkPipelineDynamicStateCreateInfo &stageInfo, std::vector<std::unique_ptr<void>> &dataPointer, void *userPoiner )
    {
        stageInfo.dynamicStateCount = 2;
        stageInfo.pDynamicStates    = new VkDynamicState[ 2 ] { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    }

    void InstanceSetup::pipelineRenderSubpass( types::pipeline pipeline, uint32_t &subpass, void *userPoiner )
    {
    }

    void InstanceSetup::pipelineBaseIndex( types::pipeline pipeline, int32_t &basePipelineIndex, void *userPoiner )
    {
    }

    pipeline::pipeline() {}
    pipeline::pipeline( types::device device, types::layout layout, std::vector<types::shader> shaders, types::pass pass )
    {
        DEFINE_DATA_FIELD
        data->device     = device;
        data->layout     = layout;
        data->renderpass = pass;
        VkPipelineVertexInputStateCreateInfo vertexCI { .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
        std::vector<void *> pvertexCI;
        device->data->window->data->instance->data->setup->pipelineVertexInputState( this, vertexCI, pvertexCI, data->device->data->window->data->instance->data->userPointer );
        VkPipelineInputAssemblyStateCreateInfo assemblyCI { .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
        std::vector<void *> passemblyCI;
        device->data->window->data->instance->data->setup->pipelineInputAssemblyState( this, assemblyCI, passemblyCI, data->device->data->window->data->instance->data->userPointer );
        VkPipelineTessellationStateCreateInfo TessellationCI { .sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO };
        std::vector<void *> pTessellationCI;
        device->data->window->data->instance->data->setup->pipelineTessellationState( this, TessellationCI, pTessellationCI, data->device->data->window->data->instance->data->userPointer );
        VkPipelineViewportStateCreateInfo ViewportCI { .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
        std::vector<void *> pViewportCI;
        device->data->window->data->instance->data->setup->pipelineViewportState( this, ViewportCI, pViewportCI, data->device->data->window->data->instance->data->userPointer );
        VkPipelineRasterizationStateCreateInfo rasterizationCI { .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
        std::vector<void *> prasterizationCI;
        device->data->window->data->instance->data->setup->pipelineRasterizationState( this, rasterizationCI, prasterizationCI, data->device->data->window->data->instance->data->userPointer );
        VkPipelineMultisampleStateCreateInfo MultisamplingCI { .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
        std::vector<void *> pMultisamplingCI;
        device->data->window->data->instance->data->setup->pipelineMultisampleState( this, MultisamplingCI, pMultisamplingCI, data->device->data->window->data->instance->data->userPointer );
        VkPipelineDepthStencilStateCreateInfo DepthCI { .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
        std::vector<void *> pDepthCI;
        device->data->window->data->instance->data->setup->pipelineDepthStencilState( this, DepthCI, pDepthCI, data->device->data->window->data->instance->data->userPointer );
        VkPipelineColorBlendStateCreateInfo cBlendCI { .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
        std::vector<void *> pcBlendCI;
        device->data->window->data->instance->data->setup->pipelineColorBlendState( this, cBlendCI, pcBlendCI, data->device->data->window->data->instance->data->userPointer );
        VkPipelineDynamicStateCreateInfo dStateCI { .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
        std::vector<void *> pdStateCI;
        device->data->window->data->instance->data->setup->pipelineDynamicState( this, dStateCI, pdStateCI, data->device->data->window->data->instance->data->userPointer );

        std::vector<VkPipelineShaderStageCreateInfo> ShaderStages;
        ShaderStages.reserve( shaders.size() );
        for ( auto &shader : shaders )
            ShaderStages.emplace_back();

        VkGraphicsPipelineCreateInfo GraphicPipeLineCreateInfo {};
        GraphicPipeLineCreateInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        GraphicPipeLineCreateInfo.stageCount          = ShaderStages.size();
        GraphicPipeLineCreateInfo.pStages             = ShaderStages.data();
        GraphicPipeLineCreateInfo.pVertexInputState   = &vertexCI;
        GraphicPipeLineCreateInfo.pInputAssemblyState = &assemblyCI;
        GraphicPipeLineCreateInfo.pViewportState      = &ViewportCI;
        GraphicPipeLineCreateInfo.pRasterizationState = &rasterizationCI;
        GraphicPipeLineCreateInfo.pDepthStencilState  = &DepthCI;
        GraphicPipeLineCreateInfo.pMultisampleState   = &MultisamplingCI;
        GraphicPipeLineCreateInfo.pColorBlendState    = &cBlendCI;
        GraphicPipeLineCreateInfo.pDynamicState       = &dStateCI;
        GraphicPipeLineCreateInfo.layout              = data->layout->data->handle;
        GraphicPipeLineCreateInfo.renderPass          = data->renderpass->data->handle;
        data->device->data->window->data->instance->data->setup->pipelineFlags( this, GraphicPipeLineCreateInfo.subpass, data->device->data->window->data->instance->data->userPointer );
        CHECK_RESULT( vkCreateGraphicsPipelines( data->device->data->device, nullptr, 1, &GraphicPipeLineCreateInfo, ALLOCATION_CALLBACK, &data->handle ) ); // TODO pipeline cachae
    }

    pipeline::~pipeline()
    {
        vkDestroyPipeline( data->device->data->device, data->handle, ALLOCATION_CALLBACK );
        vkDestroyPipelineLayout( data->device->data->device, data->layout->data->handle, ALLOCATION_CALLBACK );
    }
} // namespace Engine
