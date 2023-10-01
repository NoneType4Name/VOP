#include <RHI.hxx>
#include <device.hxx>
#include <model.hxx>
#include <pipeline.hxx>

namespace Engine
{
    namespace tools
    {
        pipeline::pipeline( std::vector<shaderID> )
        {
            // VkPipelineShaderStageCreateInfo VertexShaderStage{};
            // VertexShaderStage.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            // VertexShaderStage.stage  = VK_SHADER_STAGE_VERTEX_BIT;
            // VertexShaderStage.module = _shaders[ AppSettings.VertexShaderPath ] ? _shaders[ AppSettings.VertexShaderPath ] : _shaders[ AppSettings.VertexShaderPath ] = tools::loadShader( device, AppSettings.VertexShaderPath );
            // VertexShaderStage.pName  = "main";

            // VkPipelineShaderStageCreateInfo FragmentShaderStage{};
            // FragmentShaderStage.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            // FragmentShaderStage.stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
            // FragmentShaderStage.module = _shaders[ AppSettings.FragmentShaderPath ] ? _shaders[ AppSettings.FragmentShaderPath ] : _shaders[ AppSettings.FragmentShaderPath ] = tools::loadShader( device, AppSettings.FragmentShaderPath );
            // FragmentShaderStage.pName  = "main";

            // VkPipelineShaderStageCreateInfo ShaderStage[]{ VertexShaderStage, FragmentShaderStage };

            VkDynamicState dStates[]{ VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

            VkPipelineDynamicStateCreateInfo dStatescreateInfo{};
            dStatescreateInfo.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
            dStatescreateInfo.dynamicStateCount = sizeof( dStates ) / sizeof( dStates[ 0 ] );
            dStatescreateInfo.pDynamicStates    = dStates;

            auto bindingDescription    = vertexInputBindingDescription();
            auto attributeDescriptions = vertexInputAttributeDescription();
            VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo{};
            vertexInputCreateInfo.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            vertexInputCreateInfo.vertexBindingDescriptionCount   = 1;
            vertexInputCreateInfo.pVertexBindingDescriptions      = &bindingDescription;
            vertexInputCreateInfo.vertexAttributeDescriptionCount = attributeDescriptions.size();
            vertexInputCreateInfo.pVertexAttributeDescriptions    = attributeDescriptions.data();

            VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo{};
            inputAssemblyCreateInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            inputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;
            inputAssemblyCreateInfo.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

            VkPipelineViewportStateCreateInfo viewportState{};
            viewportState.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
            viewportState.viewportCount = 1;
            viewportState.scissorCount  = 1;

            VkPipelineRasterizationStateCreateInfo Rasterizer{};
            Rasterizer.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            Rasterizer.depthClampEnable        = VK_FALSE;
            Rasterizer.rasterizerDiscardEnable = VK_FALSE;
            Rasterizer.polygonMode             = VK_POLYGON_MODE_FILL;
            Rasterizer.lineWidth               = 1.f;
            Rasterizer.cullMode                = VK_CULL_MODE_NONE;
            Rasterizer.frontFace               = VK_FRONT_FACE_COUNTER_CLOCKWISE;
            Rasterizer.depthBiasEnable         = VK_FALSE;

            VkPipelineMultisampleStateCreateInfo Multisampling{};
            Multisampling.sType                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            Multisampling.sampleShadingEnable  = VK_FALSE;
            Multisampling.rasterizationSamples = static_cast<VkSampleCountFlagBits>( getSettings().settings.MultiSamplingCount );
            Multisampling.sampleShadingEnable  = VK_TRUE;
            Multisampling.minSampleShading     = .4f;

            VkPipelineDepthStencilStateCreateInfo DepthStencilStateCreateInfo{};
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

            VkPipelineColorBlendAttachmentState colorBlendAttachment{};
            colorBlendAttachment.colorWriteMask      = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
            colorBlendAttachment.blendEnable         = VK_FALSE;
            colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;  // Optional
            colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
            colorBlendAttachment.colorBlendOp        = VK_BLEND_OP_ADD;      // Optional
            colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;  // Optional
            colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
            colorBlendAttachment.alphaBlendOp        = VK_BLEND_OP_ADD;      // Optional

            VkPipelineColorBlendStateCreateInfo colorBlending{};
            colorBlending.sType               = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
            colorBlending.logicOpEnable       = VK_FALSE;
            colorBlending.logicOp             = VK_LOGIC_OP_COPY; // Optional
            colorBlending.attachmentCount     = 1;
            colorBlending.pAttachments        = &colorBlendAttachment;
            colorBlending.blendConstants[ 0 ] = 0.0f; // Optional
            colorBlending.blendConstants[ 1 ] = 0.0f; // Optional
            colorBlending.blendConstants[ 2 ] = 0.0f; // Optional
            colorBlending.blendConstants[ 3 ] = 0.0f; // Optional

            VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
            pipelineLayoutInfo.sType          = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipelineLayoutInfo.setLayoutCount = 1;
            pipelineLayoutInfo.pSetLayouts    = &DescriptorsSetLayout;

            Result = vkCreatePipelineLayout( device, &pipelineLayoutInfo, nullptr, &PipelineLayout.first );

            VkGraphicsPipelineCreateInfo GraphicPipeLineCreateInfo{};
            GraphicPipeLineCreateInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            GraphicPipeLineCreateInfo.stageCount          = sizeof( ShaderStage ) / sizeof( ShaderStage[ 0 ] );
            GraphicPipeLineCreateInfo.pStages             = ShaderStage;
            GraphicPipeLineCreateInfo.pVertexInputState   = &vertexInputCreateInfo;
            GraphicPipeLineCreateInfo.pInputAssemblyState = &inputAssemblyCreateInfo;
            GraphicPipeLineCreateInfo.pViewportState      = &viewportState;
            GraphicPipeLineCreateInfo.pRasterizationState = &Rasterizer;
            GraphicPipeLineCreateInfo.pDepthStencilState  = &DepthStencilStateCreateInfo;
            GraphicPipeLineCreateInfo.pMultisampleState   = &Multisampling;
            GraphicPipeLineCreateInfo.pColorBlendState    = &colorBlending;
            GraphicPipeLineCreateInfo.pDynamicState       = &dStatescreateInfo;
            GraphicPipeLineCreateInfo.layout              = PipelineLayout.first;
            GraphicPipeLineCreateInfo.renderPass          = renderpass.first;
            GraphicPipeLineCreateInfo.subpass             = 0;

            Result = vkCreateGraphicsPipelines( device, nullptr, 1, &GraphicPipeLineCreateInfo, nullptr, &Pipeline.first );
        }

        const pipelineID pipeline::getID() const
        {
            return id;
        }

        VkPipeline pipeline::getHandle() const
        {
            return ptr;
        }

        pipeline::~pipeline()
        {
            vkDestroyPipeline( getDevice(), ptr, ALLOCATION_CALLBACK );
        }
    }; // namespace tools
} // namespace Engine
