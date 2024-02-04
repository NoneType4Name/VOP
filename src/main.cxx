// #include <tiny_obj_loader.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <engine.hxx>
#include <ehi.hxx>

std::unique_ptr<Engine::instance> engine { new Engine::instance { "test", 0 } };
auto wnd { new Engine::window::window { engine.get(), { 800, 600, "test", 0, 1 } } };
auto device { new Engine::device { engine->getDevices()[ 0 ], { wnd } } };
auto swapchain { device->getLink( wnd ) };
auto commandPool { new Engine::commandPool { device->universalQueue, 0 } };
auto commandBuffer { new Engine::commandBuffer { commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY } };

struct Vertex
{
    glm::vec3 coordinate;
    glm::vec4 color;
    glm::vec2 texture;
    static VkVertexInputBindingDescription InputBindingDescription()
    {
        VkVertexInputBindingDescription VertexInputBindingDescription {};
        VertexInputBindingDescription.binding   = 0;
        VertexInputBindingDescription.stride    = sizeof( Vertex );
        VertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return VertexInputBindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 3> InputAttributeDescription()
    {
        std::array<VkVertexInputAttributeDescription, 3> VertexInputAttributeDescription {};
        VertexInputAttributeDescription[ 0 ].binding  = 0;
        VertexInputAttributeDescription[ 0 ].location = 0;
        VertexInputAttributeDescription[ 0 ].format   = VK_FORMAT_R32G32B32_SFLOAT;
        VertexInputAttributeDescription[ 0 ].offset   = offsetof( Vertex, coordinate );

        VertexInputAttributeDescription[ 1 ].binding  = 0;
        VertexInputAttributeDescription[ 1 ].location = 1;
        VertexInputAttributeDescription[ 1 ].format   = VK_FORMAT_R32G32B32A32_SFLOAT;
        VertexInputAttributeDescription[ 1 ].offset   = offsetof( Vertex, color );

        VertexInputAttributeDescription[ 2 ].binding  = 0;
        VertexInputAttributeDescription[ 2 ].location = 2;
        VertexInputAttributeDescription[ 2 ].format   = VK_FORMAT_R32G32_SFLOAT;
        VertexInputAttributeDescription[ 2 ].offset   = offsetof( Vertex, texture );
        return VertexInputAttributeDescription;
    }
    bool operator==( const Vertex &other ) const
    {
        return coordinate == other.coordinate && color == other.color && texture == other.texture;
    }
};

namespace std
{
    template<>
    struct hash<Vertex>
    {
        size_t operator()( Vertex const &vertex ) const
        {
            return ( ( hash<glm::vec3>()( vertex.coordinate ) ^
                       ( hash<glm::vec3>()( vertex.color ) << 1 ) ) >>
                     1 ) ^
                   ( hash<glm::vec2>()( vertex.texture ) << 1 );
        }
    };
} // namespace std

struct Model
{
    std::vector<Vertex> vertecies;
    std::vector<uint32_t> indecies;
    Engine::types::buffer buffer;
} primitive;

struct DemensionUniformObject
{
    alignas( 16 ) glm::mat4 model;
    alignas( 16 ) glm::mat4 view;
    alignas( 16 ) glm::mat4 proj;
};

VkDescriptorSetLayout dLayout;
std::vector<VkDescriptorSet> dSet;
std::vector<Engine::types::buffer> uniformBuffers;
VkDescriptorPool dPool;
VkPipelineLayout PipelineLayout;
VkFence fence;
VkSampler sampler;

int main()
{
    VkSamplerCreateInfo SamplerCreateInfo {};
    SamplerCreateInfo.sType     = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    SamplerCreateInfo.magFilter = VK_FILTER_LINEAR;
    SamplerCreateInfo.minFilter = VK_FILTER_LINEAR;

    SamplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    SamplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    SamplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;

    SamplerCreateInfo.anisotropyEnable = VK_TRUE;
    SamplerCreateInfo.maxAnisotropy    = device->data->description->data->properties.limits.maxSamplerAnisotropy;
    SamplerCreateInfo.borderColor      = VK_BORDER_COLOR_INT_OPAQUE_BLACK;

    SamplerCreateInfo.compareEnable = VK_FALSE;
    SamplerCreateInfo.compareOp     = VK_COMPARE_OP_ALWAYS;

    SamplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    SamplerCreateInfo.mipLodBias = .0f;
    SamplerCreateInfo.minLod     = .0f;
    SamplerCreateInfo.maxLod     = 1;

    CHECK_RESULT( vkCreateSampler( device->handle, &SamplerCreateInfo, ENGINE_ALLOCATION_CALLBACK, &sampler ) );
    VkFenceCreateInfo fCI { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
    vkCreateFence( commandPool->data->queue->data->device->handle, &fCI, ENGINE_ALLOCATION_CALLBACK, &fence );
    commandBuffer->begin();

    const char *path { "./assets/textures/rectangle/model.png" };
    int imgW, imgH, imgCs;
    stbi_uc *content { stbi_load( path, &imgW, &imgH, &imgCs, STBI_rgb_alpha ) };
    VkDeviceSize Size = imgW * imgH * 4;
    std::vector<char> img { content, content + Size };
    VkBufferCreateInfo bCI {};
    bCI.size        = img.size();
    bCI.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bCI.usage       = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    auto transferBuffer { Engine::buffer { device, bCI, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT } };
    transferBuffer.write( img );

    // attahcments
    VkImageCreateInfo ImageCreateInfo {};
    ImageCreateInfo.usage         = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    ImageCreateInfo.tiling        = VK_IMAGE_TILING_OPTIMAL;
    ImageCreateInfo.samples       = VK_SAMPLE_COUNT_2_BIT;
    ImageCreateInfo.imageType     = VK_IMAGE_TYPE_2D;
    ImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    ImageCreateInfo.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;
    ImageCreateInfo.format        = swapchain->format.format;
    ImageCreateInfo.extent        = swapchain->images[ 0 ].image->properties.extent;
    ImageCreateInfo.mipLevels     = 1;
    ImageCreateInfo.arrayLayers   = 1;

    VkImageViewCreateInfo ImageViewCreateInfo {};
    ImageViewCreateInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
    ImageViewCreateInfo.format                          = swapchain->format.format;
    ImageViewCreateInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    ImageViewCreateInfo.subresourceRange.baseMipLevel   = 0;
    ImageViewCreateInfo.subresourceRange.levelCount     = 1;
    ImageViewCreateInfo.subresourceRange.layerCount     = 1;
    auto colorImage { new Engine::image { device, ImageCreateInfo, ImageViewCreateInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT } };
    ImageCreateInfo.samples    = VK_SAMPLE_COUNT_1_BIT;
    ImageCreateInfo.usage      = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    ImageCreateInfo.format     = VK_FORMAT_R8G8B8A8_SRGB;
    ImageViewCreateInfo.format = ImageCreateInfo.format;
    auto texture { new Engine::image( device, ImageCreateInfo, ImageViewCreateInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT ) };
    texture->transition(
        commandBuffer,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        0,
        VK_ACCESS_TRANSFER_WRITE_BIT,
        VK_DEPENDENCY_BY_REGION_BIT,
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_IMAGE_ASPECT_COLOR_BIT );
    VkBufferImageCopy BufferImageCopy {};

    BufferImageCopy.bufferImageHeight = 0;
    BufferImageCopy.bufferOffset      = 0;
    BufferImageCopy.bufferRowLength   = 0;

    BufferImageCopy.imageExtent.width  = imgW;
    BufferImageCopy.imageExtent.height = imgH;
    BufferImageCopy.imageExtent.depth  = 1;
    BufferImageCopy.imageOffset        = { 0, 0, 0 };

    BufferImageCopy.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    BufferImageCopy.imageSubresource.layerCount     = 1;
    BufferImageCopy.imageSubresource.baseArrayLayer = 0;
    BufferImageCopy.imageSubresource.mipLevel       = 0;
    vkCmdCopyBufferToImage( commandBuffer->handle, transferBuffer.handle, texture->handle, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &BufferImageCopy );

    ImageCreateInfo.usage                           = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    ImageCreateInfo.format                          = device->formatPriority( { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT }, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT );
    ImageCreateInfo.samples                         = VK_SAMPLE_COUNT_2_BIT;
    ImageCreateInfo.initialLayout                   = VK_IMAGE_LAYOUT_UNDEFINED;
    ImageViewCreateInfo.format                      = ImageCreateInfo.format;
    ImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    auto depthImage { new Engine::image { device, ImageCreateInfo, ImageViewCreateInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT } };
    depthImage->transition( commandBuffer, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 0, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT, VK_DEPENDENCY_BY_REGION_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, VK_IMAGE_ASPECT_DEPTH_BIT );
    commandBuffer->submit( fence );

    // renderpass

    VkAttachmentDescription ColorAttachment {};
    ColorAttachment.format         = swapchain->format.format;
    ColorAttachment.samples        = VK_SAMPLE_COUNT_2_BIT;
    ColorAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
    ColorAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
    ColorAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    ColorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    ColorAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
    ColorAttachment.finalLayout    = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription ColorAttachmentResolve {};
    ColorAttachmentResolve.format         = swapchain->format.format;
    ColorAttachmentResolve.samples        = VK_SAMPLE_COUNT_1_BIT;
    ColorAttachmentResolve.loadOp         = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    ColorAttachmentResolve.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
    ColorAttachmentResolve.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    ColorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    ColorAttachmentResolve.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
    ColorAttachmentResolve.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentDescription DepthAttachment {};
    DepthAttachment.format         = VK_FORMAT_D32_SFLOAT;
    DepthAttachment.samples        = VK_SAMPLE_COUNT_2_BIT;
    DepthAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
    DepthAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    DepthAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    DepthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    DepthAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
    DepthAttachment.finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription Attachments[] { ColorAttachment, DepthAttachment, ColorAttachmentResolve };

    VkAttachmentReference ColorAttachmentRef {};
    ColorAttachmentRef.attachment = 0;
    ColorAttachmentRef.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference DepthAttachmentRef {};
    DepthAttachmentRef.attachment = 1;
    DepthAttachmentRef.layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference ColorAttachmentResolveRef {};
    ColorAttachmentResolveRef.attachment = 2;
    ColorAttachmentResolveRef.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass {};
    subpass.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount    = 1;
    subpass.pColorAttachments       = &ColorAttachmentRef;
    subpass.pDepthStencilAttachment = &DepthAttachmentRef;
    subpass.pResolveAttachments     = &ColorAttachmentResolveRef;

    VkSubpassDependency dependency {};
    dependency.srcSubpass    = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass    = 0;
    dependency.srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo {};
    renderPassInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = sizeof( Attachments ) / sizeof( Attachments[ 0 ] );
    renderPassInfo.pAttachments    = Attachments;
    renderPassInfo.subpassCount    = 1;
    renderPassInfo.pSubpasses      = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies   = &dependency;
    auto renderpass { new Engine::renderPass { device, renderPassInfo } };
    std::vector<Engine::types::framebuffer> frameBuffers;
    frameBuffers.reserve( swapchain->images.size() );
    for ( const auto &swpImg : swapchain->images )
        frameBuffers.emplace_back( new Engine::framebuffer { renderpass, { colorImage, depthImage, swpImg.image } } );

    // descriptor set

    VkDescriptorSetLayoutBinding DescriptorSetLayoutBindingUB {};
    DescriptorSetLayoutBindingUB.binding         = 0;
    DescriptorSetLayoutBindingUB.descriptorCount = 1;
    DescriptorSetLayoutBindingUB.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    DescriptorSetLayoutBindingUB.stageFlags      = VK_SHADER_STAGE_ALL_GRAPHICS;

    VkDescriptorSetLayoutBinding DescriptorSetLayoutBindingSampler {};
    DescriptorSetLayoutBindingSampler.binding         = 1;
    DescriptorSetLayoutBindingSampler.descriptorCount = 1;
    DescriptorSetLayoutBindingSampler.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    DescriptorSetLayoutBindingSampler.stageFlags      = VK_SHADER_STAGE_FRAGMENT_BIT;

    std::array<VkDescriptorSetLayoutBinding, 2> Binds { DescriptorSetLayoutBindingUB, DescriptorSetLayoutBindingSampler };
    VkDescriptorSetLayoutCreateInfo DescriptorSetLayoutCreateInfo {};
    DescriptorSetLayoutCreateInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    DescriptorSetLayoutCreateInfo.bindingCount = Binds.size();
    DescriptorSetLayoutCreateInfo.pBindings    = Binds.data();
    CHECK_RESULT( vkCreateDescriptorSetLayout( device->handle, &DescriptorSetLayoutCreateInfo, ENGINE_ALLOCATION_CALLBACK, &dLayout ) );

    VkDescriptorPoolSize UBDescriptorPoolSize {};
    UBDescriptorPoolSize.type            = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    UBDescriptorPoolSize.descriptorCount = swapchain->images.size();

    VkDescriptorPoolSize SamplerDescriptorPoolSize {};
    SamplerDescriptorPoolSize.type            = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    SamplerDescriptorPoolSize.descriptorCount = swapchain->images.size();

    VkDescriptorPoolSize DescriptorPoolSize[ 2 ] { UBDescriptorPoolSize, SamplerDescriptorPoolSize };

    VkDescriptorPoolCreateInfo DescriptorPoolCreateInfo {};
    DescriptorPoolCreateInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    DescriptorPoolCreateInfo.poolSizeCount = sizeof( DescriptorPoolSize ) / sizeof( DescriptorPoolSize[ 0 ] );
    DescriptorPoolCreateInfo.pPoolSizes    = DescriptorPoolSize;
    DescriptorPoolCreateInfo.maxSets       = swapchain->images.size();
    CHECK_RESULT( vkCreateDescriptorPool( device->handle, &DescriptorPoolCreateInfo, ENGINE_ALLOCATION_CALLBACK, &dPool ) );

    std::vector<VkDescriptorSetLayout> DescriptorSetLayouts( swapchain->images.size(), dLayout );
    VkDescriptorSetAllocateInfo DescriptorSetAllocateInfo {};
    DescriptorSetAllocateInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    DescriptorSetAllocateInfo.descriptorSetCount = swapchain->images.size();
    DescriptorSetAllocateInfo.pSetLayouts        = DescriptorSetLayouts.data();
    DescriptorSetAllocateInfo.descriptorPool     = dPool;

    dSet.resize( swapchain->images.size() );
    CHECK_RESULT( vkAllocateDescriptorSets( device->handle, &DescriptorSetAllocateInfo, dSet.data() ) );
    for ( size_t i { 0 }; i < swapchain->images.size(); i++ )
    {
        bCI.size  = sizeof( DemensionUniformObject );
        bCI.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        uniformBuffers.emplace_back( new Engine::buffer( device, bCI, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT ) );
        VkDescriptorBufferInfo DescriptorBufferInfo {};
        DescriptorBufferInfo.buffer = uniformBuffers[ i ]->handle;
        DescriptorBufferInfo.offset = 0;
        DescriptorBufferInfo.range  = VK_WHOLE_SIZE;

        VkWriteDescriptorSet WriteUBDescriptorSet {};
        WriteUBDescriptorSet.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        WriteUBDescriptorSet.dstSet          = dSet[ i ];
        WriteUBDescriptorSet.dstBinding      = 0;
        WriteUBDescriptorSet.dstArrayElement = 0;
        WriteUBDescriptorSet.descriptorCount = 1;
        WriteUBDescriptorSet.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        WriteUBDescriptorSet.pBufferInfo     = &DescriptorBufferInfo;

        VkDescriptorImageInfo DescriptorImageInfo {};
        DescriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        DescriptorImageInfo.imageView   = texture->view.handle;
        DescriptorImageInfo.sampler     = sampler;

        VkWriteDescriptorSet WriteSamplerDescriptorSet {};
        WriteSamplerDescriptorSet.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        WriteSamplerDescriptorSet.dstSet          = dSet[ i ];
        WriteSamplerDescriptorSet.dstBinding      = 1;
        WriteSamplerDescriptorSet.dstArrayElement = 0;
        WriteSamplerDescriptorSet.descriptorCount = 1;
        WriteSamplerDescriptorSet.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        WriteSamplerDescriptorSet.pImageInfo      = &DescriptorImageInfo;

        VkWriteDescriptorSet WriteDescriptorSet[] { WriteUBDescriptorSet, WriteSamplerDescriptorSet };
        vkUpdateDescriptorSets( device->handle, sizeof( WriteDescriptorSet ) / sizeof( WriteDescriptorSet[ 0 ] ), WriteDescriptorSet, 0, nullptr );
    }

    // pipeline

    VkPipelineShaderStageCreateInfo VertexShaderStage {};
    VertexShaderStage.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    VertexShaderStage.stage  = VK_SHADER_STAGE_VERTEX_BIT;
    VertexShaderStage.module = device->loadShader( "assets/shaders/binary.vert.spv" );
    VertexShaderStage.pName  = "main";

    VkPipelineShaderStageCreateInfo FragmentShaderStage {};
    FragmentShaderStage.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    FragmentShaderStage.stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
    FragmentShaderStage.module = device->loadShader( "assets/shaders/binary.frag.spv" );
    FragmentShaderStage.pName  = "main";

    auto ShaderStage = { VertexShaderStage, FragmentShaderStage };

    VkDynamicState dStates[] { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR, VK_DYNAMIC_STATE_VERTEX_INPUT_BINDING_STRIDE };

    VkPipelineDynamicStateCreateInfo dStatescreateInfo {};
    dStatescreateInfo.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dStatescreateInfo.dynamicStateCount = sizeof( dStates ) / sizeof( dStates[ 0 ] );
    dStatescreateInfo.pDynamicStates    = dStates;

    VkVertexInputBindingDescription VertexInputBindingDescription {};
    VertexInputBindingDescription.binding   = 0;
    VertexInputBindingDescription.stride    = sizeof( Vertex );
    VertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    auto attributeDescriptions              = Vertex::InputAttributeDescription();
    VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo {};
    vertexInputCreateInfo.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputCreateInfo.vertexBindingDescriptionCount   = 1;
    vertexInputCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>( attributeDescriptions.size() );
    vertexInputCreateInfo.pVertexBindingDescriptions      = &VertexInputBindingDescription;
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
    Multisampling.rasterizationSamples = VK_SAMPLE_COUNT_2_BIT;
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
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable    = VK_FALSE;

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

    VkPipelineLayoutCreateInfo pipelineLayoutInfo {};
    pipelineLayoutInfo.sType          = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts    = &dLayout;

    CHECK_RESULT( vkCreatePipelineLayout( device->handle, &pipelineLayoutInfo, ENGINE_ALLOCATION_CALLBACK, &PipelineLayout ) );

    VkGraphicsPipelineCreateInfo GraphicPipeLineCreateInfo {};
    GraphicPipeLineCreateInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    GraphicPipeLineCreateInfo.stageCount          = ShaderStage.size();
    GraphicPipeLineCreateInfo.pStages             = ShaderStage.begin();
    GraphicPipeLineCreateInfo.pVertexInputState   = &vertexInputCreateInfo;
    GraphicPipeLineCreateInfo.pInputAssemblyState = &inputAssemblyCreateInfo;
    GraphicPipeLineCreateInfo.pViewportState      = &viewportState;
    GraphicPipeLineCreateInfo.pRasterizationState = &Rasterizer;
    GraphicPipeLineCreateInfo.pDepthStencilState  = &DepthStencilStateCreateInfo;
    GraphicPipeLineCreateInfo.pMultisampleState   = &Multisampling;
    GraphicPipeLineCreateInfo.pColorBlendState    = &colorBlending;
    GraphicPipeLineCreateInfo.pDynamicState       = &dStatescreateInfo;
    GraphicPipeLineCreateInfo.layout              = PipelineLayout;
    GraphicPipeLineCreateInfo.renderPass          = renderpass->handle;
    GraphicPipeLineCreateInfo.subpass             = 0;

    while ( !wnd->shouldClose() )
    {
        wnd->updateEvents();
    }

    vkDestroyDescriptorPool( device->handle, dPool, ENGINE_ALLOCATION_CALLBACK );
    vkDestroyDescriptorSetLayout( device->handle, dLayout, ENGINE_ALLOCATION_CALLBACK );
    vkDestroyPipelineLayout( device->handle, PipelineLayout, ENGINE_ALLOCATION_CALLBACK );
    vkDestroyFence( device->handle, fence, ENGINE_ALLOCATION_CALLBACK );
    vkDestroySampler( device->handle, sampler, ENGINE_ALLOCATION_CALLBACK );
    return 0;
}
