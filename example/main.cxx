#define TINYOBJLOADER_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION

#define GLFW_EXPOSE_NATIVE_WIN32

#define VK_USE_PLATFORM_WIN32_KHR

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include "defenitions.hxx"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <tiny_obj_loader.h>

#include <stb_image.h>

#include <chrono>

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
    Engine::types::buffer verteciesBuffer;
    Engine::types::buffer indeciesBuffer;
    Engine::types::image texture;
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
std::vector<Engine::types::framebuffer> frameBuffers;
VkDescriptorPool dPool;
VkPipelineLayout PipelineLayout;
VkPipeline pipeline;
std::vector<VkSemaphore> imageSemaphores;
std::vector<VkSemaphore> renderedSemaphores;
std::vector<VkFence> renderedFences;
std::vector<Engine::types::commandBuffer> renderCommandBuffers;
VkSampler sampler;
VkFence fence;

int main()
{
    glfwInit();
    glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );
    glfwWindowHint( GLFW_RESIZABLE, GLFW_TRUE );
    auto window { glfwCreateWindow( 700, 600, "title", nullptr, nullptr ) };

    std::unique_ptr<Engine::instance> engine { new Engine::examples::instance { "test", 0 } };
    auto wnd { new Engine::examples::surface { engine.get(), 700, 600, glfwGetWin32Window( window ) } };
    glfwSetWindowUserPointer( window, wnd );
    auto device { new Engine::examples::device { engine->getDevices()[ 0 ], { wnd } } };
    auto swapchain { device->getLink( wnd ) };
    auto commandPool { new Engine::commandPool { device->universalQueue, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT } };
    auto commandBuffer { new Engine::commandBuffer { commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY } };
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

    OBJECTIVE_VULKAN_CHECK_RESULT( vkCreateSampler( device->handle, &SamplerCreateInfo, OBJECTIVE_VULKAN_ALLOCATION_CALLBACK, &sampler ) );
    commandBuffer->begin();

    const char *path { "./assets/textures/test/capsule0.jpg" };
    int imgW, imgH, imgCs;
    stbi_uc *content { stbi_load( path, &imgW, &imgH, &imgCs, STBI_rgb_alpha ) };
    VkDeviceSize Size = imgW * imgH * 4;
    std::vector<char> img { content, content + Size };
    VkBufferCreateInfo bCI {};
    bCI.size        = img.size();
    bCI.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bCI.usage       = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    auto transferBuffer { new Engine::buffer { device, bCI, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT } };
    transferBuffer->write( content, Size );
    stbi_image_free( content );

    // attahcments

    VkImageCreateInfo ImageCreateInfo {};
    ImageCreateInfo.usage         = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    ImageCreateInfo.tiling        = VK_IMAGE_TILING_OPTIMAL;
    ImageCreateInfo.samples       = VK_SAMPLE_COUNT_2_BIT;
    ImageCreateInfo.imageType     = VK_IMAGE_TYPE_2D;
    ImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    ImageCreateInfo.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;
    ImageCreateInfo.format        = swapchain->format.format;
    ImageCreateInfo.extent        = swapchain->images[ 0 ]->properties.extent;
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

    ImageCreateInfo.usage                           = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    ImageCreateInfo.format                          = device->formatPriority( { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT }, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT );
    ImageViewCreateInfo.format                      = ImageCreateInfo.format;
    ImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    auto depthImage { new Engine::image { device, ImageCreateInfo, ImageViewCreateInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT } };
    depthImage->transition( commandBuffer, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 0, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT, VK_DEPENDENCY_BY_REGION_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, VK_IMAGE_ASPECT_DEPTH_BIT );

    // texture

    ImageCreateInfo.extent                          = { static_cast<uint32_t>( imgW ), static_cast<uint32_t>( imgH ), 1 };
    ImageCreateInfo.samples                         = VK_SAMPLE_COUNT_1_BIT;
    ImageCreateInfo.usage                           = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    ImageCreateInfo.format                          = VK_FORMAT_R8G8B8A8_SRGB;
    ImageViewCreateInfo.format                      = ImageCreateInfo.format;
    ImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    primitive.texture                               = new Engine::image { device, ImageCreateInfo, ImageViewCreateInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT };
    primitive.texture->transition(
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
    vkCmdCopyBufferToImage( commandBuffer->handle, transferBuffer->handle, primitive.texture->handle, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &BufferImageCopy );
    primitive.texture->transition( commandBuffer,
                                   VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                   VK_ACCESS_TRANSFER_WRITE_BIT,
                                   VK_ACCESS_SHADER_READ_BIT,
                                   0,
                                   VK_PIPELINE_STAGE_TRANSFER_BIT,
                                   VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                   VK_IMAGE_ASPECT_COLOR_BIT );

    VkFenceCreateInfo fCI { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, 0, VK_FENCE_CREATE_SIGNALED_BIT };
    vkCreateFence( device->handle, &fCI, OBJECTIVE_VULKAN_ALLOCATION_CALLBACK, &fence );
    commandBuffer->submit( fence );
    delete commandBuffer;
    commandBuffer = new Engine::commandBuffer { commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY };

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
    auto renderpass { new Engine::examples::renderPass { device, renderPassInfo } };
    frameBuffers.reserve( swapchain->images.size() );
    for ( const auto &swpImg : swapchain->images )
        frameBuffers.emplace_back( new Engine::examples::framebuffer { renderpass, { colorImage, depthImage, swpImg } } );

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
    OBJECTIVE_VULKAN_CHECK_RESULT( vkCreateDescriptorSetLayout( device->handle, &DescriptorSetLayoutCreateInfo, OBJECTIVE_VULKAN_ALLOCATION_CALLBACK, &dLayout ) );

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
    OBJECTIVE_VULKAN_CHECK_RESULT( vkCreateDescriptorPool( device->handle, &DescriptorPoolCreateInfo, OBJECTIVE_VULKAN_ALLOCATION_CALLBACK, &dPool ) );

    std::vector<VkDescriptorSetLayout> DescriptorSetLayouts( swapchain->images.size(), dLayout );
    VkDescriptorSetAllocateInfo DescriptorSetAllocateInfo {};
    DescriptorSetAllocateInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    DescriptorSetAllocateInfo.descriptorSetCount = swapchain->images.size();
    DescriptorSetAllocateInfo.pSetLayouts        = DescriptorSetLayouts.data();
    DescriptorSetAllocateInfo.descriptorPool     = dPool;

    dSet.resize( swapchain->images.size() );
    OBJECTIVE_VULKAN_CHECK_RESULT( vkAllocateDescriptorSets( device->handle, &DescriptorSetAllocateInfo, dSet.data() ) );
    bCI.size  = sizeof( DemensionUniformObject );
    bCI.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    imageSemaphores.resize( swapchain->images.size() );
    renderedSemaphores.resize( swapchain->images.size() );
    renderedFences.resize( swapchain->images.size() );
    renderCommandBuffers.resize( swapchain->images.size() );
    VkSemaphoreCreateInfo sCI { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
    for ( size_t i { 0 }; i < swapchain->images.size(); i++ )
    {
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
        DescriptorImageInfo.imageView   = primitive.texture->view.handle;
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
        vkCreateSemaphore( device->handle, &sCI, OBJECTIVE_VULKAN_ALLOCATION_CALLBACK, &imageSemaphores[ i ] );
        vkCreateSemaphore( device->handle, &sCI, OBJECTIVE_VULKAN_ALLOCATION_CALLBACK, &renderedSemaphores[ i ] );
        vkCreateFence( device->handle, &fCI, OBJECTIVE_VULKAN_ALLOCATION_CALLBACK, &renderedFences[ i ] );
        renderCommandBuffers[ i ] = new Engine::commandBuffer { commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY };
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

    VkDynamicState dStates[] { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

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

    OBJECTIVE_VULKAN_CHECK_RESULT( vkCreatePipelineLayout( device->handle, &pipelineLayoutInfo, OBJECTIVE_VULKAN_ALLOCATION_CALLBACK, &PipelineLayout ) );

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
    vkCreateGraphicsPipelines( device->handle, nullptr, 1, &GraphicPipeLineCreateInfo, OBJECTIVE_VULKAN_ALLOCATION_CALLBACK, &pipeline );

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;
    if ( !tinyobj::LoadObj( &attrib, &shapes, &materials, &warn, &err, "assets/models/test/capsule.obj" ) )
    {
        SPDLOG_CRITICAL( "Failed to Load model warning:\t{}\nerror:\t{}.", warn, err );
    }
    if ( warn.length() )
        SPDLOG_WARN( "Warn in load model: {}", warn );
    if ( err.length() )
        SPDLOG_ERROR( "Error in load model: {}", err );
    std::unordered_map<Vertex, uint32_t> uniqueVertices {};
    std::vector<uint32_t> indecies {};
    for ( const auto &shape : shapes )
    {
        for ( const auto &index : shape.mesh.indices )
        {
            Vertex vertex {};

            vertex.coordinate = {
                attrib.vertices[ 3 * index.vertex_index + 0 ],
                attrib.vertices[ 3 * index.vertex_index + 1 ],
                attrib.vertices[ 3 * index.vertex_index + 2 ] };

            vertex.texture = {
                attrib.texcoords[ 2 * index.texcoord_index + 0 ],
                attrib.texcoords[ 2 * index.texcoord_index + 1 ] };

            vertex.color = { 1.0f, 1.0f, 1.0f, 1.f };

            if ( uniqueVertices.count( vertex ) == 0 )
            {
                uniqueVertices[ vertex ] = uniqueVertices.size();
                primitive.vertecies.emplace_back( vertex );
            }
            primitive.indecies.emplace_back( uniqueVertices[ vertex ] );
        }
    }
    uniqueVertices.clear();
    delete transferBuffer;
    bCI.usage                 = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    bCI.size                  = sizeof( Vertex ) * primitive.vertecies.size();
    transferBuffer            = new Engine::buffer { device, bCI, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT };
    bCI.usage                 = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    primitive.verteciesBuffer = new Engine::buffer { device, bCI, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT };
    transferBuffer->write( primitive.vertecies.data(), primitive.vertecies.size() * sizeof( Vertex ) );
    commandBuffer->begin();
    VkBufferCopy vCI {};
    vCI.srcOffset = 0;
    vCI.dstOffset = 0;
    vCI.size      = bCI.size;
    vkCmdCopyBuffer( commandBuffer->handle, transferBuffer->handle, primitive.verteciesBuffer->handle, 1, &vCI );
    delete transferBuffer;
    bCI.usage                = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    bCI.size                 = sizeof( uint32_t ) * primitive.indecies.size();
    transferBuffer           = new Engine::buffer { device, bCI, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT };
    bCI.usage                = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    primitive.indeciesBuffer = new Engine::buffer { device, bCI, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT };
    transferBuffer->write( primitive.indecies.data(), primitive.indecies.size() * sizeof( uint32_t ) );
    vCI.srcOffset = 0;
    vCI.dstOffset = 0;
    vCI.size      = bCI.size;
    vkCmdCopyBuffer( commandBuffer->handle, transferBuffer->handle, primitive.indeciesBuffer->handle, 1, &vCI );
    commandBuffer->submit( fence );

    uint32_t currentFrame { 0 };
    uint32_t image { ~0Ui32 };
    while ( !glfwWindowShouldClose( window ) )
    {
        glfwPollEvents();
        vkWaitForFences( device->handle, 1, &renderedFences[ currentFrame ], VK_TRUE, UINT64_MAX );
        do
        {
            switch ( vkAcquireNextImageKHR( device->handle, swapchain->handle, UINT64_MAX, imageSemaphores[ currentFrame ], nullptr, &image ) )
            {
                case VK_SUCCESS:
                    break;
                case VK_SUBOPTIMAL_KHR:
                reCreateSwapchain:
                case VK_ERROR_OUT_OF_DATE_KHR:
                    image        = ~0Ui32;
                    currentFrame = 0;
                    int w, h;
                    glfwGetFramebufferSize( window, &w, &h );
                    while ( !( w || h ) )
                    {
                        glfwGetFramebufferSize( window, &w, &h );
                        glfwWaitEvents();
                    }
                    wnd->updateResolution( w, h );
                    vkDeviceWaitIdle( device->handle );
                    for ( uint32_t i { 0 }; i < swapchain->images.size(); ++i )
                    {
                        delete frameBuffers[ i ];
                        delete renderCommandBuffers[ i ];
                        delete uniformBuffers[ i ];
                        vkDestroySemaphore( device->handle, imageSemaphores[ i ], OBJECTIVE_VULKAN_ALLOCATION_CALLBACK );
                        vkDestroySemaphore( device->handle, renderedSemaphores[ i ], OBJECTIVE_VULKAN_ALLOCATION_CALLBACK );
                        vkDestroyFence( device->handle, renderedFences[ i ], OBJECTIVE_VULKAN_ALLOCATION_CALLBACK );
                    }
                    vkDestroyDescriptorPool( device->handle, dPool, OBJECTIVE_VULKAN_ALLOCATION_CALLBACK );
                    swapchain->reCreate();
                    delete depthImage;
                    delete colorImage;

                    UBDescriptorPoolSize.type            = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                    UBDescriptorPoolSize.descriptorCount = swapchain->images.size();

                    SamplerDescriptorPoolSize.type            = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                    SamplerDescriptorPoolSize.descriptorCount = swapchain->images.size();

                    DescriptorPoolSize[ 0 ] = UBDescriptorPoolSize;
                    DescriptorPoolSize[ 1 ] = SamplerDescriptorPoolSize;

                    DescriptorPoolCreateInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
                    DescriptorPoolCreateInfo.poolSizeCount = sizeof( DescriptorPoolSize ) / sizeof( DescriptorPoolSize[ 0 ] );
                    DescriptorPoolCreateInfo.pPoolSizes    = DescriptorPoolSize;
                    DescriptorPoolCreateInfo.maxSets       = swapchain->images.size();
                    OBJECTIVE_VULKAN_CHECK_RESULT( vkCreateDescriptorPool( device->handle, &DescriptorPoolCreateInfo, OBJECTIVE_VULKAN_ALLOCATION_CALLBACK, &dPool ) );

                    DescriptorSetLayouts.clear();
                    DescriptorSetLayouts.resize( swapchain->images.size(), dLayout );
                    DescriptorSetAllocateInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
                    DescriptorSetAllocateInfo.descriptorSetCount = swapchain->images.size();
                    DescriptorSetAllocateInfo.pSetLayouts        = DescriptorSetLayouts.data();
                    DescriptorSetAllocateInfo.descriptorPool     = dPool;
                    dSet.resize( swapchain->images.size() );
                    OBJECTIVE_VULKAN_CHECK_RESULT( vkAllocateDescriptorSets( device->handle, &DescriptorSetAllocateInfo, dSet.data() ) );

                    delete commandBuffer;
                    commandBuffer                 = new Engine::commandBuffer { commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY };
                    ImageCreateInfo.usage         = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
                    ImageCreateInfo.tiling        = VK_IMAGE_TILING_OPTIMAL;
                    ImageCreateInfo.samples       = VK_SAMPLE_COUNT_2_BIT;
                    ImageCreateInfo.imageType     = VK_IMAGE_TYPE_2D;
                    ImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                    ImageCreateInfo.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;
                    ImageCreateInfo.format        = swapchain->format.format;
                    ImageCreateInfo.extent        = swapchain->images[ 0 ]->properties.extent;
                    ImageCreateInfo.mipLevels     = 1;
                    ImageCreateInfo.arrayLayers   = 1;

                    ImageViewCreateInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
                    ImageViewCreateInfo.format                          = swapchain->format.format;
                    ImageViewCreateInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
                    ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
                    ImageViewCreateInfo.subresourceRange.baseMipLevel   = 0;
                    ImageViewCreateInfo.subresourceRange.levelCount     = 1;
                    ImageViewCreateInfo.subresourceRange.layerCount     = 1;
                    colorImage                                          = new Engine::image { device, ImageCreateInfo, ImageViewCreateInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT };

                    ImageCreateInfo.usage                           = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
                    ImageCreateInfo.format                          = device->formatPriority( { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT }, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT );
                    ImageViewCreateInfo.format                      = ImageCreateInfo.format;
                    ImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
                    depthImage                                      = new Engine::image { device, ImageCreateInfo, ImageViewCreateInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT };
                    depthImage->transition( commandBuffer, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 0, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT, VK_DEPENDENCY_BY_REGION_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, VK_IMAGE_ASPECT_DEPTH_BIT );
                    commandBuffer->submit( fence );
                    uniformBuffers.resize( swapchain->images.size() );
                    frameBuffers.resize( swapchain->images.size() );
                    imageSemaphores.resize( swapchain->images.size() );
                    renderedSemaphores.resize( swapchain->images.size() );
                    renderedFences.resize( swapchain->images.size() );
                    renderCommandBuffers.resize( swapchain->images.size() );
                    bCI.size  = sizeof( DemensionUniformObject );
                    bCI.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
                    for ( uint32_t i { 0 }; i < swapchain->images.size(); ++i )
                    {
                        uniformBuffers[ i ] = new Engine::buffer( device, bCI, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT );
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
                        DescriptorImageInfo.imageView   = primitive.texture->view.handle;
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
                        renderCommandBuffers[ i ] = new Engine::commandBuffer { commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY };
                        frameBuffers[ i ]         = new Engine::examples::framebuffer { renderpass, { colorImage, depthImage, swapchain->images[ i ] } };
                        vkCreateSemaphore( device->handle, &sCI, OBJECTIVE_VULKAN_ALLOCATION_CALLBACK, &imageSemaphores[ i ] );
                        vkCreateSemaphore( device->handle, &sCI, OBJECTIVE_VULKAN_ALLOCATION_CALLBACK, &renderedSemaphores[ i ] );
                        vkCreateFence( device->handle, &fCI, OBJECTIVE_VULKAN_ALLOCATION_CALLBACK, &renderedFences[ i ] );
                    }

                    vkDeviceWaitIdle( device->handle );
                    break;
                default:
                    SPDLOG_CRITICAL( "Failed to acqueire image." );
                    break;
            }
        } while ( image == ~0Ui32 );
        vkResetFences( device->handle, 1, &renderedFences[ currentFrame ] );
        vkResetCommandBuffer( renderCommandBuffers[ currentFrame ]->handle, 0 );
        DemensionUniformObject Obj {};
        static auto time { std::chrono::high_resolution_clock::now() };
        auto cTime = std::chrono::high_resolution_clock::now();
        float delta { std::chrono::duration<float, std::chrono::seconds::period>( cTime - time ).count() };
        Obj.model = glm::rotate( glm::mat4( 1.f ), glm::radians( 90.f ) * delta, glm::vec3( 1.0f, 1.0f, 1.0f ) );
        Obj.view  = glm::lookAt( glm::vec3( .0f, .0f, -2.f ), glm::vec3( .0f, .0f, .0f ), glm::vec3( .0f, 1.0f, .0f ) ); // Y = -Y
        Obj.proj  = glm::perspective( glm::radians( 90.f ), swapchain->properties.capabilities.currentExtent.width / static_cast<float>( swapchain->properties.capabilities.currentExtent.height ), .01f, 2000.f );
        uniformBuffers[ currentFrame ]->write( &Obj, sizeof( Obj ) );
        renderCommandBuffers[ currentFrame ]->begin();
        VkRenderPassBeginInfo RenderPassBeginInfo {};
        RenderPassBeginInfo.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        RenderPassBeginInfo.renderPass        = renderpass->handle;
        RenderPassBeginInfo.framebuffer       = frameBuffers[ currentFrame ]->handle;
        RenderPassBeginInfo.renderArea.offset = { 0, 0 };
        RenderPassBeginInfo.renderArea.extent = { swapchain->images[ currentFrame ]->properties.extent.width, swapchain->images[ currentFrame ]->properties.extent.height };
        VkClearValue ClsClrImgBuffer { 0.5f, .5f, .5f, 1.f };
        VkClearValue ClsClrDepthImgBuffer {};
        ClsClrDepthImgBuffer.depthStencil = { 1.f, 0 };
        VkClearValue ClsClr[] { ClsClrImgBuffer, ClsClrDepthImgBuffer };
        RenderPassBeginInfo.clearValueCount = 2;
        RenderPassBeginInfo.pClearValues    = ClsClr;

        vkCmdBeginRenderPass( renderCommandBuffers[ currentFrame ]->handle, &RenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE );
        vkCmdBindPipeline( renderCommandBuffers[ currentFrame ]->handle, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline );

        VkViewport Viewport {};
        Viewport.x        = .0f;
        Viewport.y        = .0f;
        Viewport.width    = swapchain->properties.capabilities.currentExtent.width;
        Viewport.height   = swapchain->properties.capabilities.currentExtent.height;
        Viewport.minDepth = .0f;
        Viewport.maxDepth = 1.f;
        vkCmdSetViewport( renderCommandBuffers[ currentFrame ]->handle, 0, 1, &Viewport );

        VkRect2D Scissor {};
        Scissor.offset = { 0, 0 };
        Scissor.extent = swapchain->properties.capabilities.currentExtent;
        vkCmdSetScissor( renderCommandBuffers[ currentFrame ]->handle, 0, 1, &Scissor );

        VkDeviceSize offset[] { 0 };
        vkCmdBindDescriptorSets( renderCommandBuffers[ currentFrame ]->handle, VK_PIPELINE_BIND_POINT_GRAPHICS, PipelineLayout, 0, 1, &dSet[ currentFrame ], 0, nullptr );
        vkCmdBindVertexBuffers( renderCommandBuffers[ currentFrame ]->handle, 0, 1, &primitive.verteciesBuffer->handle, offset );
        vkCmdBindIndexBuffer( renderCommandBuffers[ currentFrame ]->handle, primitive.indeciesBuffer->handle, 0, VK_INDEX_TYPE_UINT32 );
        vkCmdDrawIndexed( renderCommandBuffers[ currentFrame ]->handle, primitive.indecies.size(), 1, 0, 0, 0 );
        vkCmdEndRenderPass( renderCommandBuffers[ currentFrame ]->handle );
        renderCommandBuffers[ currentFrame ]->end();
        VkPipelineStageFlags waitStages { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        VkSubmitInfo submitInfo {};
        submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount   = 1;
        submitInfo.pWaitSemaphores      = &imageSemaphores[ currentFrame ];
        submitInfo.pWaitDstStageMask    = &waitStages;
        submitInfo.commandBufferCount   = 1;
        submitInfo.pCommandBuffers      = &renderCommandBuffers[ currentFrame ]->handle;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores    = &renderedSemaphores[ currentFrame ];
        OBJECTIVE_VULKAN_CHECK_RESULT( vkQueueSubmit( device->universalQueue->handle, 1, &submitInfo, renderedFences[ currentFrame ] ) );
        VkPresentInfoKHR PresentInfo {};
        PresentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        PresentInfo.waitSemaphoreCount = 1;
        PresentInfo.pWaitSemaphores    = &renderedSemaphores[ currentFrame ];
        PresentInfo.swapchainCount     = 1;
        PresentInfo.pSwapchains        = &swapchain->handle;
        PresentInfo.pImageIndices      = &image;
        if ( vkQueuePresentKHR( device->universalQueue->handle, &PresentInfo ) != VK_SUCCESS ) goto reCreateSwapchain;
        currentFrame = ++currentFrame % swapchain->images.size();
        image        = ~0Ui32;
    }
    vkDeviceWaitIdle( device->handle );
    for ( uint32_t i { 0 }; i < swapchain->images.size(); ++i )
    {
        vkDestroySemaphore( device->handle, imageSemaphores[ i ], OBJECTIVE_VULKAN_ALLOCATION_CALLBACK );
        vkDestroySemaphore( device->handle, renderedSemaphores[ i ], OBJECTIVE_VULKAN_ALLOCATION_CALLBACK );
        vkDestroyFence( device->handle, renderedFences[ i ], OBJECTIVE_VULKAN_ALLOCATION_CALLBACK );
    }
    vkDestroyFence( device->handle, fence, OBJECTIVE_VULKAN_ALLOCATION_CALLBACK );
    vkDestroyDescriptorPool( device->handle, dPool, OBJECTIVE_VULKAN_ALLOCATION_CALLBACK );
    vkDestroyDescriptorSetLayout( device->handle, dLayout, OBJECTIVE_VULKAN_ALLOCATION_CALLBACK );
    vkDestroyPipelineLayout( device->handle, PipelineLayout, OBJECTIVE_VULKAN_ALLOCATION_CALLBACK );
    vkDestroyPipeline( device->handle, pipeline, OBJECTIVE_VULKAN_ALLOCATION_CALLBACK );
    vkDestroySampler( device->handle, sampler, OBJECTIVE_VULKAN_ALLOCATION_CALLBACK );
    glfwTerminate();
    return 0;
}
