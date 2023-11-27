// #include <RHI.hxx>
#include <device.hxx>
#include <model.hxx>
#include <pipeline.hxx>
#include <renderpass.hxx>

namespace Engine
{
    void regConstantRange( VkPushConstantRange range )
    {
        _const_ranges.push_back( range );
    }

    pipeline::pipeline( PipelineInfo info ) :
        id { ++pipeline_id }, info { info }
    {
        _pipelines[ id ] = this;
    }

    const pipelineID pipeline::getID() const
    {
        return id;
    }

    VkPipeline pipeline::getHandle() const
    {
        return Pipeline;
    }

    void pipeline::init()
    {
        std::vector<VkPipelineShaderStageCreateInfo> ShaderStages;
        ShaderStages.reserve( info.shadersID.size() );
        for ( auto &shader : info.shadersID )
            ShaderStages.push_back( getShader( shader )->getInfo() );
        VkDynamicState dStates[] { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
        VkPipelineDynamicStateCreateInfo dStatescreateInfo {};
        dStatescreateInfo.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dStatescreateInfo.dynamicStateCount = sizeof( dStates ) / sizeof( dStates[ 0 ] );
        dStatescreateInfo.pDynamicStates    = dStates;

        auto bindingDescription    = vertexInputBindingDescription();
        auto attributeDescriptions = vertexInputAttributeDescription();
        VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo {};
        vertexInputCreateInfo.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputCreateInfo.vertexBindingDescriptionCount   = 1;
        vertexInputCreateInfo.pVertexBindingDescriptions      = &bindingDescription;
        vertexInputCreateInfo.vertexAttributeDescriptionCount = attributeDescriptions.size();
        vertexInputCreateInfo.pVertexAttributeDescriptions    = attributeDescriptions.data();

        VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo {};
        inputAssemblyCreateInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;
        inputAssemblyCreateInfo.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

        VkPipelineViewportStateCreateInfo viewportState {};
        viewportState.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.scissorCount  = 1;

        VkPipelineRasterizationStateCreateInfo Rasterizer {};
        Rasterizer.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        Rasterizer.depthClampEnable        = VK_FALSE;
        Rasterizer.rasterizerDiscardEnable = VK_FALSE;
        Rasterizer.polygonMode             = VK_POLYGON_MODE_FILL;
        Rasterizer.lineWidth               = 1.f;
        Rasterizer.cullMode                = VK_CULL_MODE_NONE;
        Rasterizer.frontFace               = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        Rasterizer.depthBiasEnable         = VK_FALSE;

        VkPipelineMultisampleStateCreateInfo Multisampling {};
        Multisampling.sType                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        Multisampling.sampleShadingEnable  = VK_FALSE;
        Multisampling.rasterizationSamples = static_cast<VkSampleCountFlagBits>( getSettings().settings.MultiSamplingCount );
        Multisampling.sampleShadingEnable  = VK_TRUE;
        Multisampling.minSampleShading     = .4f;

        VkPipelineDepthStencilStateCreateInfo DepthStencilStateCreateInfo {};
        DepthStencilStateCreateInfo.sType            = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        DepthStencilStateCreateInfo.depthTestEnable  = VK_TRUE;
        DepthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
        DepthStencilStateCreateInfo.depthCompareOp   = VK_COMPARE_OP_LESS;

        DepthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
        DepthStencilStateCreateInfo.minDepthBounds        = 0.f;
        DepthStencilStateCreateInfo.maxDepthBounds        = 1.f;

        DepthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
        // DepthStencilStateCreateInfo.front             = {};
        // DepthStencilStateCreateInfo.back              = {};

        VkPipelineColorBlendAttachmentState colorBlendAttachment {};
        colorBlendAttachment.colorWriteMask      = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable         = VK_FALSE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;  // Optional
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        colorBlendAttachment.colorBlendOp        = VK_BLEND_OP_ADD;      // Optional
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;  // Optional
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        colorBlendAttachment.alphaBlendOp        = VK_BLEND_OP_ADD;      // Optional

        VkPipelineColorBlendStateCreateInfo colorBlending {};
        colorBlending.sType               = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable       = VK_FALSE;
        colorBlending.logicOp             = VK_LOGIC_OP_COPY; // Optional
        colorBlending.attachmentCount     = 1;
        colorBlending.pAttachments        = &colorBlendAttachment;
        colorBlending.blendConstants[ 0 ] = 0.0f; // Optional
        colorBlending.blendConstants[ 1 ] = 0.0f; // Optional
        colorBlending.blendConstants[ 2 ] = 0.0f; // Optional
        colorBlending.blendConstants[ 3 ] = 0.0f; // Optional
        std::vector<VkDescriptorSetLayout> descriptorLayout {};
        std::vector<descriptorSet *> descriptorSets {};
        descriptorLayout.reserve( getDescriptorSets( descriptorSets ) );
        for ( auto descriptor : descriptorSets )
            descriptorLayout.push_back( descriptor->getLayout() );
        VkPipelineLayoutCreateInfo pipelineLayoutInfo {};
        pipelineLayoutInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount         = 1;
        pipelineLayoutInfo.pSetLayouts            = descriptorLayout.data();
        pipelineLayoutInfo.pushConstantRangeCount = _const_ranges.size();
        pipelineLayoutInfo.pPushConstantRanges    = _const_ranges.data();

        CHECK_RESULT( vkCreatePipelineLayout( getDevice(), &pipelineLayoutInfo, nullptr, &PipelineLayout ) );

        VkGraphicsPipelineCreateInfo GraphicPipeLineCreateInfo {};
        GraphicPipeLineCreateInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        GraphicPipeLineCreateInfo.stageCount          = ShaderStages.size();
        GraphicPipeLineCreateInfo.pStages             = ShaderStages.data();
        GraphicPipeLineCreateInfo.pVertexInputState   = &vertexInputCreateInfo;
        GraphicPipeLineCreateInfo.pInputAssemblyState = &inputAssemblyCreateInfo;
        GraphicPipeLineCreateInfo.pViewportState      = &viewportState;
        GraphicPipeLineCreateInfo.pRasterizationState = &Rasterizer;
        GraphicPipeLineCreateInfo.pDepthStencilState  = &DepthStencilStateCreateInfo;
        GraphicPipeLineCreateInfo.pMultisampleState   = &Multisampling;
        GraphicPipeLineCreateInfo.pColorBlendState    = &colorBlending;
        GraphicPipeLineCreateInfo.pDynamicState       = &dStatescreateInfo;
        GraphicPipeLineCreateInfo.layout              = PipelineLayout;
        GraphicPipeLineCreateInfo.renderPass          = getRenderPass();
        GraphicPipeLineCreateInfo.subpass             = 0;

        CHECK_RESULT( vkCreateGraphicsPipelines( getDevice(), nullptr, 1, &GraphicPipeLineCreateInfo, nullptr, &Pipeline ) );
    }

    pipeline::~pipeline()
    {
        vkDestroyPipeline( getDevice(), Pipeline, ALLOCATION_CALLBACK );
        vkDestroyPipelineLayout( getDevice(), PipelineLayout, ALLOCATION_CALLBACK );
        _pipelines[ id ] = nullptr;
    }

    pipeline *getPipeline( pipelineID )
    {
        return _pipelines[ pipeline_id ];
    }

    void createPipelines()
    {
        for ( auto &pipe : _pipelines )
        {
            pipe.second->init();
        }
    }

    void destroyPipelines()
    {
        for ( auto &pipe : _pipelines )
        {
            delete pipe.second;
        }
    }
} // namespace Engine
