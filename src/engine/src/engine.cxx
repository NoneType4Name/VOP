#include <engine.hxx>
#include <spdlog/spdlog.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <glm/gtx/hash.hpp>
#include <tiny_obj_loader.h>
#include <glm/gtc/matrix_transform.hpp>
#include <vulkan/vk_enum_string_helper.h>
#include <format>
#include <iostream>
#include <fstream>
#define ENGINE_VERSION VK_MAKE_VERSION( 0, 0, 1 )

namespace Engine
{
    namespace
    {
        const std::vector<const char *> ValidationLayers{ "VK_LAYER_KHRONOS_validation" };
        const std::vector<const char *> RequeredDeviceExts{ VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_SHADER_NON_SEMANTIC_INFO_EXTENSION_NAME, VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME };
        uint16_t _width{ 800ui16 };
        uint16_t _height{ 600ui16 };
        uint16_t DisplayWidth;
        uint16_t DisplayHeight;
        std::string _title;
        GLFWwindow *_window{ nullptr };
        GLFWmonitor *_monitor{ nullptr };
        VkInstance _instance;
        VkSurfaceKHR _surface;
        Settings *_settings;

        struct Vertex
        {
            glm::vec3 coordinate;
            glm::vec4 color;
            glm::vec2 texture;
            static VkVertexInputBindingDescription InputBindingDescription()
            {
                VkVertexInputBindingDescription VertexInputBindingDescription{};
                VertexInputBindingDescription.binding   = 0;
                VertexInputBindingDescription.stride    = sizeof( Vertex );
                VertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
                return VertexInputBindingDescription;
            }

            static std::array<VkVertexInputAttributeDescription, 3> InputAttributeDescription()
            {
                std::array<VkVertexInputAttributeDescription, 3> VertexInputAttributeDescription{};
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

        struct Model
        {
            std::vector<Vertex> ModelVertecies;
            std::vector<uint32_t> ModelVerteciesIndices;
            uint32_t VerteciesOffset{};
            uint32_t IndeciesOffset{};
        };

        struct DemensionsUniformrObject
        {
            alignas( 16 ) glm::mat4 model;
            alignas( 16 ) glm::mat4 view;
            alignas( 16 ) glm::mat4 proj;
        };

        struct QueueFamilyIndices
        {
            std::optional<uint32_t> graphic;
            std::optional<uint32_t> present;
            std::optional<uint32_t> transfer;
            // std::optional<uint32_t> compute;

            bool isComplete()
            {
                return graphic.has_value() && present.has_value() && transfer.has_value(); // && compute.has_value()
            }
        };

        struct SwapChain
        {
            VkSwapchainKHR Swapchain;
            bool SwapchainInit{ false };
            VkSurfaceFormatKHR Format{ VK_FORMAT_UNDEFINED };
            VkPresentModeKHR PresentMode;
            VkSurfaceCapabilitiesKHR Capabilities;
            std::vector<VkSurfaceFormatKHR> AviliableFormats;
            std::vector<VkPresentModeKHR> AviliablePresentModes;
        };

        struct _physicalDevice
        {
            VkPhysicalDevice device;
            VkPhysicalDeviceMemoryProperties memoryProperties;
            VkPhysicalDeviceProperties properties;
            VkPhysicalDeviceFeatures features;
            SwapChain swapchain;
            QueueFamilyIndices Indecies;
        };

        struct _logicalDevice
        {
            _physicalDevice *physicalDevice;
            VkDevice device;
            std::pair<VkRenderPass, bool> renderpass{ nullptr, false };
            std::vector<VkImage> Images;
            std::vector<VkImageView> ImagesViews;
            std::vector<VkFramebuffer> ImageFrameBuffers;
            std::vector<VkBuffer> UniformBuffers;
            std::vector<VkDeviceMemory> UniformBuffersMemory;
            std::vector<VkCommandBuffer> CommandBuffers;
            std::vector<VkSemaphore> ImageAvailableSemaphores;
            std::vector<VkSemaphore> RenderFinishedSemaphores;
            std::vector<VkFence> WaitFrames;

            VkPhysicalDeviceFeatures enabledFeatures{};

            VkDescriptorSetLayout DescriptorsSetLayout;
            VkDescriptorPool DescriptorPool;
            VkDescriptorSet DescriptorSet;
            std::pair<VkPipelineLayout, bool> PipelineLayout{ nullptr, false };
            std::pair<VkPipeline, bool> Pipeline{ nullptr, false };
            VkCommandPool GrapchicCommandPool;
            VkImage DepthImage;
            VkImageView DepthImageView;
            VkDeviceMemory DepthImageMemory;
            VkImage ColorImage;
            VkImageView ColorImageView;
            VkDeviceMemory ColorImageMemory;

            VkImage TextureImage;
            VkDeviceMemory TextureImageMemory;
            VkImageView TextureImageView;
            VkSampler TextureSampler;

            VkFormat DepthImageFormat;
            VkQueue graphicQueue;
            VkQueue presentQueue;
            VkQueue transferQueue;
            // VkQueue computeQueue;
            _logicalDevice( _physicalDevice &pDevice )
            {
                physicalDevice = &pDevice;
                std::vector<VkDeviceQueueCreateInfo> QueuesCreateInfo{};
                std::unordered_map<uint32_t, float> QueueFamiliesPriority{
                    { physicalDevice->Indecies.graphic.value(), 1.0f },
                    { physicalDevice->Indecies.present.value(), 1.0f },
                    { physicalDevice->Indecies.transfer.value(), 1.0f },
                    // { _selectedPhysicalDevice->Indecies.compute.value(), 1.0f },
                };
                for( auto &QueueFamily : QueueFamiliesPriority )
                {
                    VkDeviceQueueCreateInfo QueueCreateInfo{};
                    QueueCreateInfo.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                    QueueCreateInfo.queueFamilyIndex = QueueFamily.first;
                    QueueCreateInfo.queueCount       = 1;
                    QueueCreateInfo.pQueuePriorities = &QueueFamily.second;
                    QueuesCreateInfo.push_back( QueueCreateInfo );
                }
                enabledFeatures.samplerAnisotropy = VK_TRUE;
                enabledFeatures.sampleRateShading = VK_TRUE;

                VkPhysicalDeviceDescriptorIndexingFeaturesEXT PhysicalDeviceDescriptorIndexingFeatures{};
                PhysicalDeviceDescriptorIndexingFeatures.sType                                     = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT;
                PhysicalDeviceDescriptorIndexingFeatures.shaderSampledImageArrayNonUniformIndexing = VK_TRUE;
                PhysicalDeviceDescriptorIndexingFeatures.runtimeDescriptorArray                    = VK_TRUE;
                PhysicalDeviceDescriptorIndexingFeatures.descriptorBindingVariableDescriptorCount  = VK_TRUE;

                VkDeviceCreateInfo createInfo{};
                createInfo.sType                = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
                createInfo.pNext                = &PhysicalDeviceDescriptorIndexingFeatures;
                createInfo.queueCreateInfoCount = static_cast<uint32_t>( QueuesCreateInfo.size() );
                createInfo.pQueueCreateInfos    = QueuesCreateInfo.data();
                createInfo.pEnabledFeatures     = &enabledFeatures;
                // createInfo.pEnabledFeatures        = &PhysicalDeviceFeatures;
                createInfo.enabledExtensionCount   = static_cast<uint32_t>( RequeredDeviceExts.size() );
                createInfo.ppEnabledExtensionNames = RequeredDeviceExts.data();
#ifdef ENGINE_DEBUG // to do
                createInfo.enabledLayerCount   = static_cast<uint32_t>( ValidationLayers.size() );
                createInfo.ppEnabledLayerNames = ValidationLayers.data();
#else
                createInfo.enabledLayerCount = 0;
#endif
                VkResult Result = vkCreateDevice( physicalDevice->device, &createInfo, nullptr, &device );
                if( Result != VK_SUCCESS )
                    SPDLOG_CRITICAL( "Failed to Create logical Device, error: {}", string_VkResult( Result ) );

                VkDescriptorSetLayoutBinding DescriptorSetLayoutBindingUB{};
                DescriptorSetLayoutBindingUB.binding         = 0;
                DescriptorSetLayoutBindingUB.descriptorCount = 1;
                DescriptorSetLayoutBindingUB.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                DescriptorSetLayoutBindingUB.stageFlags      = VK_SHADER_STAGE_ALL_GRAPHICS;

                VkDescriptorSetLayoutBinding DescriptorSetLayoutBindingSampler{};
                DescriptorSetLayoutBindingSampler.binding         = 1;
                DescriptorSetLayoutBindingSampler.descriptorCount = 1;
                DescriptorSetLayoutBindingSampler.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                DescriptorSetLayoutBindingSampler.stageFlags      = VK_SHADER_STAGE_FRAGMENT_BIT;

                VkDescriptorSetLayoutBinding Binds[]{ DescriptorSetLayoutBindingUB, DescriptorSetLayoutBindingSampler };
                VkDescriptorSetLayoutCreateInfo DescriptorSetLayoutCreateInfo{};
                DescriptorSetLayoutCreateInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
                DescriptorSetLayoutCreateInfo.bindingCount = sizeof( Binds ) / sizeof( Binds[ 0 ] );
                DescriptorSetLayoutCreateInfo.pBindings    = Binds;

                Result = vkCreateDescriptorSetLayout( device, &DescriptorSetLayoutCreateInfo, nullptr, &DescriptorsSetLayout );

                VkDescriptorPoolSize UBDescriptorPoolSize{};
                UBDescriptorPoolSize.type            = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                UBDescriptorPoolSize.descriptorCount = 1;

                VkDescriptorPoolSize SamplerDescriptorPoolSize{};
                SamplerDescriptorPoolSize.type            = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                SamplerDescriptorPoolSize.descriptorCount = 2; // textures count

                VkDescriptorPoolSize DescriptorPoolSize[ 2 ]{ UBDescriptorPoolSize, SamplerDescriptorPoolSize };

                VkDescriptorPoolCreateInfo DescriptorPoolCreateInfo{};
                DescriptorPoolCreateInfo.flags         = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;
                DescriptorPoolCreateInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
                DescriptorPoolCreateInfo.poolSizeCount = sizeof( DescriptorPoolSize ) / sizeof( DescriptorPoolSize[ 0 ] );
                DescriptorPoolCreateInfo.pPoolSizes    = DescriptorPoolSize;
                DescriptorPoolCreateInfo.maxSets       = 2;

                Result = vkCreateDescriptorPool( device, &DescriptorPoolCreateInfo, nullptr, &DescriptorPool );

                VkCommandPoolCreateInfo GrapchiCommandPoolCreateInfo{};
                GrapchiCommandPoolCreateInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
                GrapchiCommandPoolCreateInfo.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
                GrapchiCommandPoolCreateInfo.queueFamilyIndex = physicalDevice->Indecies.graphic.value();

                VkDescriptorSetAllocateInfo DescriptorSetAllocateInfo{};
                DescriptorSetAllocateInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
                DescriptorSetAllocateInfo.descriptorSetCount = 1;
                DescriptorSetAllocateInfo.pSetLayouts        = &DescriptorsSetLayout;
                DescriptorSetAllocateInfo.descriptorPool     = DescriptorPool;

                // DescriptorSets.resize( _MaxFramesInFlight );
                Result = vkAllocateDescriptorSets( device, &DescriptorSetAllocateInfo, &DescriptorSet );

                Result = vkCreateCommandPool( device, &GrapchiCommandPoolCreateInfo, nullptr, &GrapchicCommandPool );

                UniformBuffers.resize( physicalDevice->swapchain.Capabilities.maxImageCount );
                UniformBuffersMemory.resize( physicalDevice->swapchain.Capabilities.maxImageCount );
                ImageAvailableSemaphores.resize( physicalDevice->swapchain.Capabilities.maxImageCount );
                RenderFinishedSemaphores.resize( physicalDevice->swapchain.Capabilities.maxImageCount );
                WaitFrames.resize( physicalDevice->swapchain.Capabilities.maxImageCount );
                VkSemaphoreCreateInfo semaphoreInfo{};
                semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
                VkFenceCreateInfo FenceInfo{};
                FenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
                FenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

                VkBufferCreateInfo BufferCreateInfo{};
                BufferCreateInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
                BufferCreateInfo.size        = sizeof( DemensionsUniformrObject );
                BufferCreateInfo.usage       = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
                BufferCreateInfo.flags       = 0;
                BufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

                for( uint32_t i{ 0 }; i < physicalDevice->swapchain.Capabilities.maxImageCount; i++ )
                {
                    Result = vkCreateBuffer( device, &BufferCreateInfo, nullptr, &UniformBuffers[ i ] );
                    VkMemoryRequirements Requirements;
                    vkGetBufferMemoryRequirements( device, UniformBuffers[ i ], &Requirements );
                    VkMemoryAllocateInfo MemoryAllocateInfo{};
                    MemoryAllocateInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                    MemoryAllocateInfo.allocationSize  = Requirements.size;
                    MemoryAllocateInfo.memoryTypeIndex = _memoryTypeIndex( Requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT );

                    Result = vkAllocateMemory( device, &MemoryAllocateInfo, nullptr, &UniformBuffersMemory[ i ] );
                    Result = vkBindBufferMemory( device, UniformBuffers[ i ], UniformBuffersMemory[ i ], 0 );

                    Result = vkCreateSemaphore( device, &semaphoreInfo, nullptr, &ImageAvailableSemaphores[ i ] );
                    Result = vkCreateSemaphore( device, &semaphoreInfo, nullptr, &RenderFinishedSemaphores[ i ] );
                    Result = vkCreateFence( device, &FenceInfo, nullptr, &WaitFrames[ i ] );
                }
                vkGetDeviceQueue( device, physicalDevice->Indecies.graphic.value(), 0, &graphicQueue );
                vkGetDeviceQueue( device, physicalDevice->Indecies.present.value(), 0, &presentQueue );
                vkGetDeviceQueue( device, physicalDevice->Indecies.transfer.value(), 0, &transferQueue );
                CreateRender();
            }

            ~_logicalDevice()
            {
                for( auto sh : _shaderStage )
                {
                    vkDestroyShaderModule( device, sh.second, nullptr );
                }
                vkDeviceWaitIdle( device );
                vkDestroyImage( device, DepthImage, nullptr );
                vkDestroyImageView( device, DepthImageView, nullptr );
                vkFreeMemory( device, DepthImageMemory, nullptr );
                vkDestroyImage( device, ColorImage, nullptr );
                vkDestroyImageView( device, ColorImageView, nullptr );
                vkFreeMemory( device, ColorImageMemory, nullptr );
                for( uint32_t i{ 0 }; i < physicalDevice->swapchain.Capabilities.maxImageCount; i++ )
                {
                    vkDestroyBuffer( device, UniformBuffers[ i ], nullptr );
                    vkFreeMemory( device, UniformBuffersMemory[ i ], nullptr );
                    vkDestroySemaphore( device, ImageAvailableSemaphores[ i ], nullptr );
                    vkDestroySemaphore( device, RenderFinishedSemaphores[ i ], nullptr );
                    vkDestroyFence( device, WaitFrames[ i ], nullptr );
                }
                for( uint32_t i{ 0 }; i < Images.size(); i++ )
                {
                    // vkDestroyImage( device, Images[ i ], nullptr );
                    vkDestroyFramebuffer( device, ImageFrameBuffers[ i ], nullptr );
                    vkDestroyImageView( device, ImagesViews[ i ], nullptr );
                }
                vkDestroyCommandPool( device, GrapchicCommandPool, nullptr );
                vkDestroyDescriptorPool( device, DescriptorPool, nullptr );
                vkDestroyDescriptorSetLayout( device, DescriptorsSetLayout, nullptr );
                vkDestroyPipeline( device, Pipeline.first, nullptr );
                vkDestroyPipelineLayout( device, PipelineLayout.first, nullptr );
                vkDestroyRenderPass( device, renderpass.first, nullptr );
                vkDestroySwapchainKHR( device, physicalDevice->swapchain.Swapchain, nullptr );
                vkDestroyDevice( device, nullptr );
            }

            void CreateRender()
            {
                if( physicalDevice->swapchain.Format.format == VK_FORMAT_UNDEFINED )
                {
                    VkSurfaceFormatKHR SurfaceFormat{ physicalDevice->swapchain.AviliableFormats[ 0 ] };
                    for( const auto &format : physicalDevice->swapchain.AviliableFormats )
                    {
                        if( format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR ) SurfaceFormat = format;
                        break;
                    }
                    physicalDevice->swapchain.Format = SurfaceFormat;

                    VkPresentModeKHR SurfacePresentMode{ VK_PRESENT_MODE_FIFO_KHR };
                    for( const auto &mode : physicalDevice->swapchain.AviliablePresentModes )
                    {
                        if( mode == VK_PRESENT_MODE_MAILBOX_KHR ) SurfacePresentMode = mode;
                        break;
                    }
                    physicalDevice->swapchain.PresentMode = SurfacePresentMode;

                    for( auto format : std::vector<VkFormat>{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT } )
                    {
                        VkFormatProperties FormatProperties{};
                        vkGetPhysicalDeviceFormatProperties( physicalDevice->device, format, &FormatProperties );
                        if( ( FormatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT ) == VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT )
                        {
                            DepthImageFormat = format;
                            break;
                        }
                    }
                }
                if( renderpass.second )
                {
                    vkDestroyImage( device, ColorImage, nullptr );
                    for( uint32_t i{ 0 }; i < Images.size(); i++ )
                    {
                        vkDestroyImage( device, Images[ i ], nullptr );
                        vkDestroyBuffer( device, UniformBuffers[ i ], nullptr );
                        vkFreeMemory( device, UniformBuffersMemory[ i ], nullptr );
                        vkDestroyFramebuffer( device, ImageFrameBuffers[ i ], nullptr );
                        vkDestroyImageView( device, ImagesViews[ i ], nullptr );
                    }
                    vkDestroyImage( device, DepthImage, nullptr );
                }

                VkAttachmentDescription ColorAttachment{};
                ColorAttachment.format         = physicalDevice->swapchain.Format.format;
                ColorAttachment.samples        = static_cast<VkSampleCountFlagBits>( _settings->MultiSamplingCount );
                ColorAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
                ColorAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
                ColorAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                ColorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                ColorAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
                ColorAttachment.finalLayout    = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

                VkAttachmentDescription ColorAttachmentResolve{};
                ColorAttachmentResolve.format         = physicalDevice->swapchain.Format.format;
                ColorAttachmentResolve.samples        = VK_SAMPLE_COUNT_1_BIT;
                ColorAttachmentResolve.loadOp         = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                ColorAttachmentResolve.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
                ColorAttachmentResolve.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                ColorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                ColorAttachmentResolve.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
                ColorAttachmentResolve.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

                VkAttachmentDescription DepthAttachment{};
                DepthAttachment.format         = DepthImageFormat;
                DepthAttachment.samples        = static_cast<VkSampleCountFlagBits>( _settings->MultiSamplingCount );
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
                VkResult Result{ vkCreateRenderPass( device, &renderPassInfo, nullptr, &renderpass.first ) };

                // Renderpass
                VkSwapchainCreateInfoKHR SwapchainCreateInfo{};
                SwapchainCreateInfo.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
                SwapchainCreateInfo.surface          = _surface;
                SwapchainCreateInfo.minImageCount    = physicalDevice->swapchain.Capabilities.minImageCount;
                SwapchainCreateInfo.imageFormat      = physicalDevice->swapchain.Format.format;
                SwapchainCreateInfo.imageColorSpace  = physicalDevice->swapchain.Format.colorSpace;
                SwapchainCreateInfo.presentMode      = physicalDevice->swapchain.PresentMode;
                SwapchainCreateInfo.imageExtent      = physicalDevice->swapchain.Capabilities.currentExtent;
                SwapchainCreateInfo.imageArrayLayers = 1;
                SwapchainCreateInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
                uint32_t physicalDeviceIndicesValue[]{ physicalDevice->Indecies.graphic.value(), physicalDevice->Indecies.present.value() };
                if( physicalDevice->Indecies.graphic != physicalDevice->Indecies.present )
                {
                    SwapchainCreateInfo.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
                    SwapchainCreateInfo.queueFamilyIndexCount = 2;
                    SwapchainCreateInfo.pQueueFamilyIndices   = physicalDeviceIndicesValue;
                }
                else
                {
                    SwapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
                }
                SwapchainCreateInfo.preTransform   = physicalDevice->swapchain.Capabilities.currentTransform;
                SwapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
                SwapchainCreateInfo.clipped        = VK_TRUE;
                if( physicalDevice->swapchain.SwapchainInit )
                    SwapchainCreateInfo.oldSwapchain = physicalDevice->swapchain.Swapchain;
                else
                {
                    SwapchainCreateInfo.oldSwapchain        = VK_NULL_HANDLE;
                    physicalDevice->swapchain.SwapchainInit = true;
                }

                Result = vkCreateSwapchainKHR( device, &SwapchainCreateInfo, nullptr, &physicalDevice->swapchain.Swapchain );
                vkDestroySwapchainKHR( device, SwapchainCreateInfo.oldSwapchain, nullptr );
                uint32_t c;
                vkGetSwapchainImagesKHR( device, physicalDevice->swapchain.Swapchain, &c, nullptr );
                Images.resize( c );
                vkGetSwapchainImagesKHR( device, physicalDevice->swapchain.Swapchain, &c, Images.data() );

                // Pipeline
                VkPipelineShaderStageCreateInfo VertexShaderStage{};
                VertexShaderStage.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                VertexShaderStage.stage  = VK_SHADER_STAGE_VERTEX_BIT;
                VertexShaderStage.module = _loadShader( device, "./assets/shaders/binary.vert.spv" );
                VertexShaderStage.pName  = "main";

                VkPipelineShaderStageCreateInfo FragmentShaderStage{};
                FragmentShaderStage.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                FragmentShaderStage.stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
                FragmentShaderStage.module = _loadShader( device, "./assets/shaders/binary.frag.spv" );
                FragmentShaderStage.pName  = "main";

                VkPipelineShaderStageCreateInfo ShaderStage[]{ VertexShaderStage, FragmentShaderStage };

                VkDynamicState dStates[]{ VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

                VkPipelineDynamicStateCreateInfo dStatescreateInfo{};
                dStatescreateInfo.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
                dStatescreateInfo.dynamicStateCount = sizeof( dStates ) / sizeof( dStates[ 0 ] );
                dStatescreateInfo.pDynamicStates    = dStates;

                auto bindingDescription    = Vertex::InputBindingDescription();
                auto attributeDescriptions = Vertex::InputAttributeDescription();
                VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo{};
                vertexInputCreateInfo.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
                vertexInputCreateInfo.vertexBindingDescriptionCount   = 1;
                vertexInputCreateInfo.vertexAttributeDescriptionCount = attributeDescriptions.size();
                vertexInputCreateInfo.pVertexBindingDescriptions      = &bindingDescription;
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
                Multisampling.rasterizationSamples = static_cast<VkSampleCountFlagBits>( _settings->MultiSamplingCount );
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

                vkDestroyPipeline( device, Pipeline.first, nullptr );
                vkDestroyPipelineLayout( device, PipelineLayout.first, nullptr );
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

                VkImageCreateInfo ColorImageCreateInfo{};
                ColorImageCreateInfo.samples = static_cast<VkSampleCountFlagBits>( _settings->MultiSamplingCount );
                ColorImageCreateInfo.format  = physicalDevice->swapchain.Format.format;
                _createImage( physicalDevice->swapchain.Capabilities.currentExtent.width, physicalDevice->swapchain.Capabilities.currentExtent.height, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_IMAGE_TILING_OPTIMAL, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, ColorImageCreateInfo, ColorImage, ColorImageMemory, 0 );
                ColorImageView = _createImageView( ColorImage, physicalDevice->swapchain.Format.format, VK_IMAGE_ASPECT_COLOR_BIT, {} );

                VkImageCreateInfo DepthImageCreateInfo{};
                DepthImageCreateInfo.format  = DepthImageFormat;
                DepthImageCreateInfo.samples = static_cast<VkSampleCountFlagBits>( _settings->MultiSamplingCount );
                _createImage( physicalDevice->swapchain.Capabilities.currentExtent.width, physicalDevice->swapchain.Capabilities.currentExtent.height, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_IMAGE_TILING_OPTIMAL, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, DepthImageCreateInfo, DepthImage, DepthImageMemory, 0 );
                DepthImageView = _createImageView( DepthImage, DepthImageFormat, VK_IMAGE_ASPECT_DEPTH_BIT, {} );
                VkPipelineStageFlags DepthSrcStageMask{ VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT };
                VkPipelineStageFlags DepthDstStageMask{ VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT };
                VkImageMemoryBarrier DepthImageMemoryBarrier{};
                DepthImageMemoryBarrier.sType                       = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
                DepthImageMemoryBarrier.image                       = DepthImage;
                DepthImageMemoryBarrier.oldLayout                   = VK_IMAGE_LAYOUT_UNDEFINED;
                DepthImageMemoryBarrier.newLayout                   = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                DepthImageMemoryBarrier.srcQueueFamilyIndex         = VK_QUEUE_FAMILY_IGNORED;
                DepthImageMemoryBarrier.dstQueueFamilyIndex         = VK_QUEUE_FAMILY_IGNORED;
                DepthImageMemoryBarrier.srcAccessMask               = 0;
                DepthImageMemoryBarrier.dstAccessMask               = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                DepthImageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
                if( DepthImageFormat == VK_FORMAT_D32_SFLOAT_S8_UINT || DepthImageFormat == VK_FORMAT_D24_UNORM_S8_UINT )
                    DepthImageMemoryBarrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
                DepthImageMemoryBarrier.subresourceRange.layerCount     = 1;
                DepthImageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
                DepthImageMemoryBarrier.subresourceRange.levelCount     = 1;
                DepthImageMemoryBarrier.subresourceRange.baseMipLevel   = 0;

                VkCommandBuffer TransitionCommandBuffer;
                VkCommandBufferAllocateInfo CommandBufferAllocateInfo{};
                CommandBufferAllocateInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
                CommandBufferAllocateInfo.commandPool        = GrapchicCommandPool;
                CommandBufferAllocateInfo.commandBufferCount = 1;
                CommandBufferAllocateInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

                Result = vkAllocateCommandBuffers( device, &CommandBufferAllocateInfo, &TransitionCommandBuffer );
                VkCommandBufferBeginInfo CommandBufferBeginInfo{};
                CommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
                CommandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
                vkBeginCommandBuffer( TransitionCommandBuffer, &CommandBufferBeginInfo );

                vkCmdPipelineBarrier( TransitionCommandBuffer, DepthSrcStageMask, DepthDstStageMask, VK_DEPENDENCY_BY_REGION_BIT, 0, 0, 0, 0, 1, &DepthImageMemoryBarrier );

                vkEndCommandBuffer( TransitionCommandBuffer );
                VkSubmitInfo SubmitInfo{};
                SubmitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                SubmitInfo.commandBufferCount = 1;
                SubmitInfo.pCommandBuffers    = &TransitionCommandBuffer;

                Result = vkQueueSubmit( graphicQueue, 1, &SubmitInfo, nullptr );
                vkQueueWaitIdle( graphicQueue );
                vkFreeCommandBuffers( device, GrapchicCommandPool, 1, &TransitionCommandBuffer );

                ImagesViews.resize( Images.size() );
                ImageFrameBuffers.resize( Images.size() );
                VkImageViewCreateInfo ImageViewCreateInfo{};
                ImageViewCreateInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                ImageViewCreateInfo.format                          = physicalDevice->swapchain.Format.format;
                ImageViewCreateInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
                ImageViewCreateInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
                ImageViewCreateInfo.subresourceRange.baseMipLevel   = 0;
                ImageViewCreateInfo.subresourceRange.levelCount     = 1;
                ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
                ImageViewCreateInfo.subresourceRange.layerCount     = 1;
                for( size_t i{ 0 }; i < ImagesViews.size(); i++ )
                {
                    ImageViewCreateInfo.image = Images[ i ];
                    Result                    = vkCreateImageView( device, &ImageViewCreateInfo, nullptr, &ImagesViews[ i ] );

                    VkImageView Attachments[]{ ColorImageView, DepthImageView, ImagesViews[ i ] };
                    VkFramebufferCreateInfo FrameBufferCreateInfo{};
                    FrameBufferCreateInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
                    FrameBufferCreateInfo.renderPass      = renderpass.first;
                    FrameBufferCreateInfo.attachmentCount = sizeof( Attachments ) / sizeof( Attachments[ 0 ] );
                    FrameBufferCreateInfo.pAttachments    = Attachments;
                    FrameBufferCreateInfo.width           = physicalDevice->swapchain.Capabilities.currentExtent.width;
                    FrameBufferCreateInfo.height          = physicalDevice->swapchain.Capabilities.currentExtent.height;
                    FrameBufferCreateInfo.layers          = 1;
                    Result                                = vkCreateFramebuffer( device, &FrameBufferCreateInfo, nullptr, &ImageFrameBuffers[ i ] );
                }
            }

          private:
            std::unordered_map<const char *, VkShaderModule> _shaderStage;
            VkShaderModule _loadShader( VkDevice uDevice, const char *sPath )
            {
                auto AviliableShader = _shaderStage[ sPath ];
                if( AviliableShader )
                    return AviliableShader;
                std::ifstream File{ sPath, std::fstream::ate | std::fstream::binary };
                if( !File.is_open() )
                {
                    SPDLOG_CRITICAL( "Failed to open shader: {}.", sPath );
                    throw std::runtime_error( std::format( "Failed to open shader: {}.", sPath ) );
                }
                size_t shBsize{ static_cast<size_t>( File.tellg() ) };
                std::vector<char> Data( shBsize );
                File.seekg( 0 );
                File.read( Data.data(), shBsize );
                File.close();
                VkShaderModuleCreateInfo ShaderModuleCreateInfo{};
                ShaderModuleCreateInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
                ShaderModuleCreateInfo.codeSize = Data.size();
                ShaderModuleCreateInfo.pCode    = reinterpret_cast<const uint32_t *>( Data.data() );
                VkShaderModule shaderModule;
                VkResult Result{ vkCreateShaderModule( uDevice, &ShaderModuleCreateInfo, nullptr, &shaderModule ) };
                _shaderStage[ sPath ] = shaderModule;
                return shaderModule;
            }

            uint32_t _memoryTypeIndex( uint32_t type, VkMemoryPropertyFlags properties )
            {
                for( uint32_t i{ 0 }; i < physicalDevice->memoryProperties.memoryTypeCount; i++ )
                {
                    if( ( type & ( 1 << i ) ) && ( ( physicalDevice->memoryProperties.memoryTypes[ i ].propertyFlags & properties ) == properties ) ) return i;
                }
                return -1;
            }
            void _createImage( const uint32_t width, const uint32_t height, const VkBufferUsageFlags iUsage, const VkImageTiling tiling, const VkMemoryPropertyFlags mProperties, VkImageCreateInfo ImageCreateInfo, VkImage &Image, VkDeviceMemory &mImage, VkDeviceSize MemoryOffset )
            {
                ImageCreateInfo.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
                ImageCreateInfo.imageType     = VK_IMAGE_TYPE_2D;
                ImageCreateInfo.extent.width  = width;
                ImageCreateInfo.extent.height = height;
                ImageCreateInfo.extent.depth  = ImageCreateInfo.extent.depth ? ImageCreateInfo.extent.depth : 1;
                ImageCreateInfo.mipLevels     = ImageCreateInfo.mipLevels ? ImageCreateInfo.mipLevels : 1;
                ImageCreateInfo.arrayLayers   = ImageCreateInfo.arrayLayers ? ImageCreateInfo.arrayLayers : 1;
                ImageCreateInfo.format        = ImageCreateInfo.format ? ImageCreateInfo.format : VK_FORMAT_R8G8B8A8_SRGB;
                ImageCreateInfo.tiling        = tiling;
                ImageCreateInfo.initialLayout = ImageCreateInfo.initialLayout ? ImageCreateInfo.initialLayout : VK_IMAGE_LAYOUT_UNDEFINED;
                ImageCreateInfo.usage         = iUsage;
                ImageCreateInfo.sharingMode   = ImageCreateInfo.sharingMode ? ImageCreateInfo.sharingMode : VK_SHARING_MODE_EXCLUSIVE;
                ImageCreateInfo.samples       = ImageCreateInfo.samples ? ImageCreateInfo.samples : VK_SAMPLE_COUNT_1_BIT;

                VkResult Result{ vkCreateImage( device, &ImageCreateInfo, nullptr, &Image ) };

                VkMemoryRequirements MemoryRequirements{};
                vkGetImageMemoryRequirements( device, Image, &MemoryRequirements );

                VkMemoryAllocateInfo MemoryAllocateInfo{};
                MemoryAllocateInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                MemoryAllocateInfo.allocationSize  = MemoryRequirements.size;
                MemoryAllocateInfo.memoryTypeIndex = _memoryTypeIndex( MemoryRequirements.memoryTypeBits, mProperties );

                Result = vkAllocateMemory( device, &MemoryAllocateInfo, nullptr, &mImage );
                Result = vkBindImageMemory( device, Image, mImage, MemoryOffset );
            }

            VkImageView _createImageView( VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageViewCreateInfo ImageViewCreateInfo )
            {
                VkImageView ImageView;
                ImageViewCreateInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                ImageViewCreateInfo.image                           = image;
                ImageViewCreateInfo.format                          = format;
                ImageViewCreateInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
                ImageViewCreateInfo.subresourceRange.aspectMask     = aspectFlags;
                ImageViewCreateInfo.subresourceRange.baseMipLevel   = ImageViewCreateInfo.subresourceRange.baseMipLevel ? ImageViewCreateInfo.subresourceRange.baseMipLevel : 0;
                ImageViewCreateInfo.subresourceRange.levelCount     = ImageViewCreateInfo.subresourceRange.levelCount ? ImageViewCreateInfo.subresourceRange.levelCount : 1;
                ImageViewCreateInfo.subresourceRange.baseArrayLayer = ImageViewCreateInfo.subresourceRange.baseArrayLayer ? ImageViewCreateInfo.subresourceRange.baseArrayLayer : 0;
                ImageViewCreateInfo.subresourceRange.layerCount     = ImageViewCreateInfo.subresourceRange.layerCount ? ImageViewCreateInfo.subresourceRange.layerCount : 1;

                VkResult Result{ vkCreateImageView( device, &ImageViewCreateInfo, nullptr, &ImageView ) };
                return ImageView;
            }

            void ImageTransition( VkImage image, VkFormat format, VkImageLayout layout, VkImageLayout NewLayout, VkImageMemoryBarrier ImageMemoryBarrier )
            {
                VkPipelineStageFlags srcStageMask{ 0 };
                VkPipelineStageFlags dstStageMask{ 0 };
                ImageMemoryBarrier.sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
                ImageMemoryBarrier.image               = image;
                ImageMemoryBarrier.oldLayout           = layout;
                ImageMemoryBarrier.newLayout           = NewLayout;
                ImageMemoryBarrier.srcQueueFamilyIndex = ImageMemoryBarrier.srcQueueFamilyIndex ? ImageMemoryBarrier.srcQueueFamilyIndex : VK_QUEUE_FAMILY_IGNORED;
                ImageMemoryBarrier.dstQueueFamilyIndex = ImageMemoryBarrier.dstQueueFamilyIndex ? ImageMemoryBarrier.dstQueueFamilyIndex : VK_QUEUE_FAMILY_IGNORED;

                if( NewLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL )
                {
                    ImageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
                    if( format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT )
                        ImageMemoryBarrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
                }
                else
                    ImageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

                if( layout == VK_IMAGE_LAYOUT_UNDEFINED && NewLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL )
                {
                    ImageMemoryBarrier.srcAccessMask = 0;
                    ImageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                    srcStageMask                     = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
                    dstStageMask                     = VK_PIPELINE_STAGE_TRANSFER_BIT;
                }
                else if( layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && NewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL )
                {
                    ImageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                    ImageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
                    srcStageMask                     = VK_PIPELINE_STAGE_TRANSFER_BIT;
                    dstStageMask                     = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
                }
                else if( layout == VK_IMAGE_LAYOUT_UNDEFINED && NewLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL )
                {
                    ImageMemoryBarrier.srcAccessMask = 0;
                    ImageMemoryBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                    srcStageMask                     = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
                    dstStageMask                     = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
                }
                else
                {
                    SPDLOG_CRITICAL( fmt::format( "Unsupported layout transition, old layout: {}, new layout: {}", std::to_string( layout ), std::to_string( NewLayout ) ) );
                    throw std::runtime_error( std::format( "Unsupported layout transition, old layout: {}, new layout: {}", std::to_string( layout ), std::to_string( NewLayout ) ) );
                }

                ImageMemoryBarrier.subresourceRange.layerCount     = ImageMemoryBarrier.subresourceRange.layerCount ? ImageMemoryBarrier.subresourceRange.layerCount : 1;
                ImageMemoryBarrier.subresourceRange.baseArrayLayer = ImageMemoryBarrier.subresourceRange.baseArrayLayer ? ImageMemoryBarrier.subresourceRange.baseArrayLayer : 0;
                ImageMemoryBarrier.subresourceRange.levelCount     = ImageMemoryBarrier.subresourceRange.levelCount ? ImageMemoryBarrier.subresourceRange.levelCount : 1;
                ImageMemoryBarrier.subresourceRange.baseMipLevel   = ImageMemoryBarrier.subresourceRange.baseMipLevel ? ImageMemoryBarrier.subresourceRange.baseMipLevel : 0;

                VkCommandBuffer commandBuffer;
                VkCommandBufferAllocateInfo CommandBufferAllocateInfo{};
                CommandBufferAllocateInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
                CommandBufferAllocateInfo.commandPool        = GrapchicCommandPool;
                CommandBufferAllocateInfo.commandBufferCount = 1;
                CommandBufferAllocateInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
                VkResult Result{ vkAllocateCommandBuffers( device, &CommandBufferAllocateInfo, &commandBuffer ) };
                VkCommandBufferBeginInfo CommandBufferBeginInfo{};
                CommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
                CommandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
                vkBeginCommandBuffer( commandBuffer, &CommandBufferBeginInfo );

                vkCmdPipelineBarrier( commandBuffer, srcStageMask, dstStageMask, VK_DEPENDENCY_BY_REGION_BIT, 0, 0, 0, 0, 1, &ImageMemoryBarrier );
                vkEndCommandBuffer( commandBuffer );
                VkSubmitInfo SubmitInfo{};
                SubmitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                SubmitInfo.commandBufferCount = 1;
                SubmitInfo.pCommandBuffers    = &commandBuffer;

                Result = vkQueueSubmit( graphicQueue, 1, &SubmitInfo, nullptr );
                vkQueueWaitIdle( graphicQueue );
                vkFreeCommandBuffers( device, GrapchicCommandPool, 1, &commandBuffer );
            }

            void _createBuffer( VkDeviceSize size, VkBufferUsageFlags bUsage, VkMemoryPropertyFlags mProperties, VkBufferCreateInfo BufferCreateInfo, VkBuffer &Buffer, VkDeviceMemory &mBuffer, VkDeviceSize MemoryOffset )
            {
                BufferCreateInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
                BufferCreateInfo.size        = size;
                BufferCreateInfo.usage       = bUsage;
                BufferCreateInfo.flags       = BufferCreateInfo.flags ? BufferCreateInfo.flags : 0;
                BufferCreateInfo.sharingMode = BufferCreateInfo.sharingMode ? BufferCreateInfo.sharingMode : VK_SHARING_MODE_EXCLUSIVE;

                VkResult Result{ vkCreateBuffer( device, &BufferCreateInfo, nullptr, &Buffer ) };
                VkMemoryRequirements Requirements;
                vkGetBufferMemoryRequirements( device, Buffer, &Requirements );
                VkMemoryAllocateInfo MemoryAllocateInfo{};
                MemoryAllocateInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                MemoryAllocateInfo.allocationSize  = Requirements.size;
                MemoryAllocateInfo.memoryTypeIndex = _memoryTypeIndex( Requirements.memoryTypeBits, mProperties );

                Result = vkAllocateMemory( device, &MemoryAllocateInfo, nullptr, &mBuffer );
                Result = vkBindBufferMemory( device, Buffer, mBuffer, MemoryOffset );
            }
        };

        _logicalDevice *_selectedLogicalDevice;
        std::vector<_physicalDevice> _avilablePhysicalDevices;
        KeyEventCallBack _KeyEventCallBack{ nullptr };
        SettingCallBack _uSettingsCallBack{ nullptr };

        struct _
        {
            _()
            {
            }
            ~_()
            {
            }
        } _;

        void _getScreenResolution( uint16_t &width, uint16_t &height )
        {
            if( _monitor == nullptr )
                _monitor = glfwGetPrimaryMonitor();
            else
                _monitor = glfwGetWindowMonitor( _window );
            glfwGetVideoMode( _monitor );
            auto mode = glfwGetVideoMode( _monitor );
            width     = mode->width;
            height    = mode->height;
        }

        inline GrapchicPhysicalDevice _getPhysicalDeviceStruct( _physicalDevice *_device )
        {
            return { _device->properties.deviceName, GrapchiDeviceType( _device->properties.deviceType ), _device->properties.deviceID };
        };

        QueueFamilyIndices _physicalDeviceGetIndecies( VkPhysicalDevice _dev )
        {
            QueueFamilyIndices _indices;
            uint32_t _c{ 0 };
            vkGetPhysicalDeviceQueueFamilyProperties( _dev, &_c, nullptr );
            std::vector<VkQueueFamilyProperties> QueueFamilies( _c );
            vkGetPhysicalDeviceQueueFamilyProperties( _dev, &_c, QueueFamilies.data() );
            if( _c - 1 )
            {
                _c = 0;
                for( const auto &queueF : QueueFamilies )
                {
                    VkBool32 presentSupport{ false };
                    vkGetPhysicalDeviceSurfaceSupportKHR( _dev, _c, _surface, &presentSupport );
                    if( !_indices.graphic.has_value() && queueF.queueFlags & VK_QUEUE_GRAPHICS_BIT )
                    {
                        _indices.graphic = _c;
                    }
                    if( !_indices.present.has_value() && presentSupport ) _indices.present = _c;
                    else if( !_indices.transfer.has_value() && queueF.queueFlags & VK_QUEUE_TRANSFER_BIT )
                        _indices.transfer = _c;
                    // else if( !_indices.compute.has_value() && queueF.queueFlags & VK_QUEUE_COMPUTE_BIT )
                    //     _indices.compute = index;
                    else
                        break;
                    _c++;
                }
            }
            else
            {
                VkBool32 presentSupport{ false };
                vkGetPhysicalDeviceSurfaceSupportKHR( _dev, 0, _surface, &presentSupport );
                if( QueueFamilies[ 0 ].queueFlags & ( VK_QUEUE_TRANSFER_BIT | VK_QUEUE_GRAPHICS_BIT ) && presentSupport )
                {
                    _indices = { 0, 0, 0 };
                }
            }
            return _indices;
        }

        void FramebufferResizeCallback( GLFWwindow *window, int w, int h )
        {
            SetWindowResolution( w, h );
            // TO DO: update frame buffer size (recreate swapchain).
        }

        inline void WindwoResizeCallback( GLFWwindow *window, int w, int h )
        {
            FramebufferResizeCallback( window, w, h );
        }

        void _KeyCallbackManager( GLFWwindow *wnd, int key, int scancode, int action, int mods )
        {
            _KeyEventCallBack( key, scancode, action, mods );
        }

        static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT MessageLevel,
            VkDebugUtilsMessageTypeFlagsEXT MessageType,
            const VkDebugUtilsMessengerCallbackDataEXT *CallbackData,
            void *UserData )
        {
            // VulkanInstance *Vulkan = static_cast<VulkanInstance *>( UserData ); // Not used.
            std::string StrMessageType;
            switch( MessageType )
            {
                case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
                    StrMessageType = "GeneralError";
                    break;
                case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
                    StrMessageType = "SpecificationError";
                    break;
                case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
                    StrMessageType = "MisuseVulkanApiError";
                    break;

                default:
                    StrMessageType = string_VkDebugUtilsMessageTypeFlagsEXT( MessageType );
                    break;
            }
            switch( static_cast<uint32_t>( MessageLevel ) )
            {
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
                    SPDLOG_DEBUG( std::format( "{} message: {}", ( MessageType == VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT ? "" : format( "Type: {}, ", StrMessageType ) ), CallbackData->pMessage ) );
                    break;
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                    SPDLOG_INFO( std::format( "{} message: {}", ( MessageType == VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT ? "" : format( "Type: {}, ", StrMessageType ) ), CallbackData->pMessage ) );
                    break;
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                    SPDLOG_WARN( std::format( "{} message: {}", ( MessageType == VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT ? "" : format( "Type: {}, ", StrMessageType ) ), CallbackData->pMessage ) );
                    break;
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                    SPDLOG_CRITICAL( std::format( "{} message: {}", ( MessageType == VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT ? "" : format( "Type: {}, ", StrMessageType ) ), CallbackData->pMessage ) );
                    return VK_TRUE;
                    break;
            }
            return VK_FALSE;
        }

    } // namespace

    void init( uint16_t width, uint16_t height, const char *title, Settings *settings )
    {
        _settings = settings;
        spdlog::set_level( DEBUG ? spdlog::level::trace : spdlog::level::critical );
        spdlog::set_pattern( "[%H:%M:%S.%e] [%^%l%$] %v" );
        if( glfwInit() )
        {
            SPDLOG_DEBUG( "GLFW{} inititialized.", glfwGetVersionString() );
            glfwSetErrorCallback( []( int code, const char *data )
                                  { SPDLOG_ERROR( "GLFW ERROR {}: {}", code, data ); } );
        }
        else
        {
            SPDLOG_CRITICAL( "GLFW not initialized." );
            SPDLOG_INFO( "Exit with code {}.", EXIT_FAILURE );
            exit( EXIT_FAILURE );
        }
        glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );
        glfwWindowHint( GLFW_RESIZABLE, GLFW_TRUE );
        _window = glfwCreateWindow( width ? width : _width, height ? height : _height, title, nullptr, nullptr );
        _title  = title;
        SetWindowResolution( width, height );
        glfwSetFramebufferSizeCallback( _window, FramebufferResizeCallback );
        glfwSetWindowSizeCallback( _window, WindwoResizeCallback );
        CentralizeWindow();

        VkInstanceCreateInfo InstanceCreateInfo{};
        InstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        uint32_t glfwExtensionsCount{ 0 };
        const char **glfwExtensions = glfwGetRequiredInstanceExtensions( &glfwExtensionsCount );
        std::vector<const char *> glfwExtensionsVector( glfwExtensions, glfwExtensions + glfwExtensionsCount );

#ifdef _DEBUG
        uint32_t _c;
        vkEnumerateInstanceLayerProperties( &_c, nullptr );
        std::vector<VkLayerProperties> AviableLayers{ _c };
        vkEnumerateInstanceLayerProperties( &_c, AviableLayers.data() );
        size_t c{ ValidationLayers.size() };
        std::vector<const char *> NotAvilableLayers{ ValidationLayers.begin(), ValidationLayers.end() };
        for( size_t i{ 0 }; i < c; i++ )
        {
            for( uint32_t _i{ 0 }; _i < _c; _i++ )
                if( !strcmp( NotAvilableLayers[ c - 1 - i ], AviableLayers[ _i ].layerName ) )
                {
                    NotAvilableLayers.erase( NotAvilableLayers.end() - i - 1 );
                    break;
                }
            if( NotAvilableLayers.empty() )
                break;
        }
        if( !NotAvilableLayers.empty() )
        {
            std::string Err = std::format( "Not finded validation layers: {}:\n", std::to_string( NotAvilableLayers.size() ) );
            for( const auto VL : NotAvilableLayers )
            {
                Err += std::format( "\t {}\n", VL );
            }
            SPDLOG_CRITICAL( Err );
        }

        VkValidationFeatureEnableEXT enabled[]{ VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT };
        VkValidationFeaturesEXT ValidationFeatures{};
        ValidationFeatures.sType                          = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
        ValidationFeatures.enabledValidationFeatureCount  = sizeof( enabled ) / sizeof( enabled[ 0 ] );
        ValidationFeatures.pEnabledValidationFeatures     = enabled;
        ValidationFeatures.disabledValidationFeatureCount = 0;
        ValidationFeatures.pDisabledValidationFeatures    = nullptr;

        VkDebugUtilsMessengerCreateInfoEXT DebugUtilsMessengerCreateInfoEXT{};
        InstanceCreateInfo.pNext                         = &DebugUtilsMessengerCreateInfoEXT;
        DebugUtilsMessengerCreateInfoEXT.sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        DebugUtilsMessengerCreateInfoEXT.pNext           = &ValidationFeatures;
        DebugUtilsMessengerCreateInfoEXT.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        DebugUtilsMessengerCreateInfoEXT.messageType     = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        DebugUtilsMessengerCreateInfoEXT.pfnUserCallback = DebugCallback;
        // DebugUtilsMessengerCreateInfoEXT.pUserData       = this; // No used.
        glfwExtensionsVector.push_back( VK_EXT_DEBUG_UTILS_EXTENSION_NAME );
        InstanceCreateInfo.enabledLayerCount   = ValidationLayers.size();
        InstanceCreateInfo.ppEnabledLayerNames = ValidationLayers.data();

#endif
        InstanceCreateInfo.enabledExtensionCount   = glfwExtensionsVector.size();
        InstanceCreateInfo.ppEnabledExtensionNames = glfwExtensionsVector.data();
        VkApplicationInfo ApplicationInfo{};
        InstanceCreateInfo.pApplicationInfo = &ApplicationInfo;
        ApplicationInfo.sType               = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        ApplicationInfo.engineVersion       = 0;
        ApplicationInfo.apiVersion          = VK_API_VERSION_1_0;
        auto app_name{ "Grapchic engine of " + _title };
        ApplicationInfo.pApplicationName   = app_name.c_str();
        ApplicationInfo.applicationVersion = ENGINE_VERSION;
        VkResult Result{ vkCreateInstance( &InstanceCreateInfo, nullptr, &_instance ) };

#if defined( _WIN32 )
        VkWin32SurfaceCreateInfoKHR Win32SurfaceCreateInfo{
            VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
            nullptr,
            0,
            GetModuleHandle( nullptr ),
            glfwGetWin32Window( _window ) };
        Result = vkCreateWin32SurfaceKHR( _instance, &Win32SurfaceCreateInfo, nullptr, &_surface );
// #if defined( __LINUX__ )

//         VkXlibSurfaceCreateInfoKHR XlibSurfaceCreateInfoKHR{
//             VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR,
//             nullptr,
//             0,
//             &dpy,
//             &window };
//         Result = vkCreateXlibSurfaceKHR( Instance, &XlibSurfaceCreateInfoKHR, nullptr, &Screen )
#endif

        if( Result != VK_SUCCESS )
        {
            SPDLOG_CRITICAL( "Failed to Create surface, error" );
        }
        vkEnumeratePhysicalDevices( _instance, &_c, nullptr );
        std::vector<VkPhysicalDevice> _avilableDevices( _c );
        _avilablePhysicalDevices.reserve( _c );
        _avilablePhysicalDevices.clear();
        vkEnumeratePhysicalDevices( _instance, &_c, _avilableDevices.data() );
        for( auto device : _avilableDevices )
        {
            uint16_t mark{ 0 };
            SwapChain swpchnprprts;
            VkPhysicalDeviceMemoryProperties PhysicalDeviceMemoryProperties{};
            VkPhysicalDeviceProperties PhysicalDeviceProperties{};
            VkPhysicalDeviceFeatures PhysicalDeviceFeatures{};
            vkGetPhysicalDeviceProperties( device, &PhysicalDeviceProperties );
            vkGetPhysicalDeviceFeatures( device, &PhysicalDeviceFeatures );
            vkGetPhysicalDeviceMemoryProperties( device, &PhysicalDeviceMemoryProperties );

            // if( PhysicalDeviceProperties.deviceType )
            //     mark = 5 - PhysicalDeviceProperties.deviceType;

            vkGetPhysicalDeviceSurfaceCapabilitiesKHR( device, _surface, &swpchnprprts.Capabilities );
            uint32_t formatsCount{ 0 };
            vkGetPhysicalDeviceSurfaceFormatsKHR( device, _surface, &formatsCount, nullptr );
            swpchnprprts.AviliableFormats.resize( formatsCount );
            vkGetPhysicalDeviceSurfaceFormatsKHR( device, _surface, &formatsCount, swpchnprprts.AviliableFormats.data() );
            uint32_t PresentModesCount{ 0 };
            vkGetPhysicalDeviceSurfacePresentModesKHR( device, _surface, &PresentModesCount, nullptr );
            swpchnprprts.AviliablePresentModes.resize( PresentModesCount );
            vkGetPhysicalDeviceSurfacePresentModesKHR( device, _surface, &PresentModesCount, swpchnprprts.AviliablePresentModes.data() );

            _avilablePhysicalDevices.push_back( {
                device,
                PhysicalDeviceMemoryProperties,
                PhysicalDeviceProperties,
                PhysicalDeviceFeatures,
                swpchnprprts,
                _physicalDeviceGetIndecies( device ),
            } );
        }
        _selectedLogicalDevice = new _logicalDevice( _avilablePhysicalDevices[ 0 ] );
    }

    void SetGraphicDevice( GrapchicPhysicalDevice device )
    {
        for( auto dev : _avilablePhysicalDevices )
        {
            if( dev.properties.deviceID == device.deviceID )
            {
                delete _selectedLogicalDevice;
                _selectedLogicalDevice = new _logicalDevice( dev );
                return;
            }
        }
    }

    inline GrapchicPhysicalDevice GetActiveGrapchiDevice()
    {
        return { _selectedLogicalDevice->physicalDevice->properties.deviceName, GrapchiDeviceType( _selectedLogicalDevice->physicalDevice->properties.deviceType ), _selectedLogicalDevice->physicalDevice->properties.deviceID };
    }

    std::vector<GrapchicPhysicalDevice> GetGraphicDevices()
    {
        std::vector<GrapchicPhysicalDevice> _devices;
        for( const auto &_dev : _avilablePhysicalDevices )
        {
            _devices.push_back( { _dev.properties.deviceName,
                                  GrapchiDeviceType( _dev.properties.deviceType ),
                                  _dev.properties.deviceID,
                                  { _dev.properties.limits.framebufferDepthSampleCounts } } );
        }
        return _devices;
    }

    void shutdown()
    {
        delete _selectedLogicalDevice;
        vkDestroySurfaceKHR( _instance, _surface, nullptr );
        glfwDestroyWindow( _window );
        glfwTerminate();
    }

    void SetWindowResolution( uint16_t width, uint16_t height )
    {
        _getScreenResolution( DisplayWidth, DisplayHeight );
        if( width + height )
        {
            glfwSetWindowAttrib( _window, GLFW_RESIZABLE, GLFW_TRUE );
            glfwSetWindowAttrib( _window, GLFW_DECORATED, GLFW_TRUE );
        }
        else
        {

            glfwSetWindowAttrib( _window, GLFW_RESIZABLE, GLFW_FALSE );
            glfwSetWindowAttrib( _window, GLFW_DECORATED, GLFW_FALSE );
        }
        if( !width ) width = DisplayWidth;
        if( !height ) height = DisplayHeight;
        _width  = width;
        _height = height;
        glfwSetWindowSize( _window, static_cast<int>( width ), static_cast<int>( height ) );
    }

    void CentralizeWindow()
    {
        glfwSetWindowPos( _window, ( DisplayWidth / 2 ) - ( _width / 2 ), ( DisplayHeight / 2 ) - ( _height / 2 ) );
    }

    void SetWindowTitle( const char *new_title )
    {
        _title = new_title;
        glfwSetWindowTitle( _window, new_title );
    }
    inline void SetWindowTitle( std::string &new_title )
    {
        SetWindowTitle( new_title.c_str() );
    }

    KeyEventCallBack SetKeyEventsCallback( KeyEventCallBack Callback )
    {
        KeyEventCallBack prvsClbck{ _KeyEventCallBack };
        glfwSetKeyCallback( _window, _KeyCallbackManager );
        _KeyEventCallBack = Callback;
        return _KeyEventCallBack;
    }

    inline bool WindowShouldClose()
    {
        return glfwWindowShouldClose( _window );
    }
    inline void UpdateEvents()
    {
        glfwPollEvents();
    }

} // namespace Engine
