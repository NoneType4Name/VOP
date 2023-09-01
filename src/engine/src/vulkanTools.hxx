#include <engine.hxx>
#include <spdlog/spdlog.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <glm/gtx/hash.hpp>
#include <tiny_obj_loader.h>
#include <glm/gtc/matrix_transform.hpp>
#include <vulkan/vk_enum_string_helper.h>
#include <fstream>

namespace Engine
{
    namespace
    {
        namespace tools
        {
            struct vertex
            {
                glm::vec3 coordinate;
                glm::vec4 color;
                glm::vec2 texture;
                bool operator==( const vertex &other ) const
                {
                    return coordinate == other.coordinate && color == other.color && texture == other.texture;
                }
            };

            struct model
            {
                std::vector<vertex> ModelVertecies;
                std::vector<uint32_t> ModelVerteciesIndices;
                uint32_t VerteciesOffset{};
                uint32_t IndeciesOffset{};
            };

            struct uniformrObject
            {
                alignas( 16 ) glm::mat4 model;
                alignas( 16 ) glm::mat4 view;
                alignas( 16 ) glm::mat4 proj;
            };

            struct queueFamilyIndices
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

            struct swapChain
            {
                VkSwapchainKHR Swapchain;
                bool SwapchainInit{ false };
                VkSurfaceFormatKHR Format{ VK_FORMAT_UNDEFINED };
                VkPresentModeKHR PresentMode;
                VkSurfaceCapabilitiesKHR Capabilities;
                std::vector<VkSurfaceFormatKHR> AviliableFormats;
                std::vector<VkPresentModeKHR> AviliablePresentModes;
            };

            struct physicalDevice
            {
                VkPhysicalDevice device;
                VkPhysicalDeviceMemoryProperties memoryProperties;
                VkPhysicalDeviceProperties properties;
                VkPhysicalDeviceFeatures features;
                swapChain swapchain;
                queueFamilyIndices Indecies;
            };

            inline VkDeviceQueueCreateInfo queueCreateInfo( uint32_t index, uint32_t count, const float *priority )
            {
                return { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, 0, 0, index, count, priority };
            };

            inline VkDescriptorSetLayoutBinding descriptorSetLayoutBinding( VkDescriptorType type, VkShaderStageFlags stageFlags, uint32_t binding, uint32_t descriptorCount = 1 )
            {
                return { binding, type, descriptorCount, stageFlags };
            }

            inline VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo( const std::vector<VkDescriptorSetLayoutBinding> &bindings )
            {
                return { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO, 0, 0, static_cast<uint32_t>( bindings.size() ), bindings.data() };
            }

            inline VkDescriptorPoolSize descriptorPoolSize( VkDescriptorType type, uint32_t descriptorCount )
            {
                return { type, descriptorCount };
            }

            inline VkDescriptorPoolCreateInfo descriptorPoolCreateInfo( const std::vector<VkDescriptorPoolSize> &poolSizes, uint32_t maxSets )
            {
                return { VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO, 0, 0, maxSets, static_cast<uint32_t>( poolSizes.size() ), poolSizes.data() };
            }

            inline VkCommandPoolCreateInfo commandPoolCreateInfo( VkCommandPoolCreateFlags flags, uint32_t queueFamilyIndex )
            {
                return { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO, 0, flags, queueFamilyIndex };
            }

            inline VkDescriptorSetAllocateInfo descriptorSetAllocateInfo( VkDescriptorPool descriptorPool, const VkDescriptorSetLayout *pSetLayouts, uint32_t descriptorSetCount )
            {
                return { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO, 0, descriptorPool, descriptorSetCount, pSetLayouts };
            }

            inline VkSemaphoreCreateInfo semaphoreCreateInfo()
            {
                return { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO, 0, 0 };
            }

            inline VkFenceCreateInfo fenceCreateInfo( VkFenceCreateFlags flags = 0 )
            {
                return { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, 0, flags };
            }

            inline VkMemoryAllocateInfo memoryAllocateInfo()
            {
                return { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
            }

            inline VkBufferCreateInfo bufferCreateInfo( VkBufferUsageFlags usage, VkDeviceSize size, VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE, const std::vector<uint32_t> &queueFamilyIndices = {} )
            {
                return { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO, 0, 0, size, usage, sharingMode, static_cast<uint32_t>( queueFamilyIndices.size() ), queueFamilyIndices.data() };
            }

            inline static VkVertexInputBindingDescription getVertexBindingDescription()
            {
                VkVertexInputBindingDescription VertexInputBindingDescription{};
                VertexInputBindingDescription.binding   = 0;
                VertexInputBindingDescription.stride    = sizeof( tools::vertex );
                VertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
                return VertexInputBindingDescription;
            }

            inline static std::array<VkVertexInputAttributeDescription, 3> getVertexAttributeDescription()
            {
                std::array<VkVertexInputAttributeDescription, 3> VertexInputAttributeDescription{};
                VertexInputAttributeDescription[ 0 ].binding  = 0;
                VertexInputAttributeDescription[ 0 ].location = 0;
                VertexInputAttributeDescription[ 0 ].format   = VK_FORMAT_R32G32B32_SFLOAT;
                VertexInputAttributeDescription[ 0 ].offset   = offsetof( tools::vertex, coordinate );

                VertexInputAttributeDescription[ 1 ].binding  = 0;
                VertexInputAttributeDescription[ 1 ].location = 1;
                VertexInputAttributeDescription[ 1 ].format   = VK_FORMAT_R32G32B32A32_SFLOAT;
                VertexInputAttributeDescription[ 1 ].offset   = offsetof( tools::vertex, color );

                VertexInputAttributeDescription[ 2 ].binding  = 0;
                VertexInputAttributeDescription[ 2 ].location = 2;
                VertexInputAttributeDescription[ 2 ].format   = VK_FORMAT_R32G32_SFLOAT;
                VertexInputAttributeDescription[ 2 ].offset   = offsetof( tools::vertex, texture );
                return VertexInputAttributeDescription;
            }

            inline uint32_t memoryTypeIndex( uint32_t type, VkMemoryPropertyFlags properties, VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties )
            {
                for( uint32_t i{ 0 }; i < physicalDeviceMemoryProperties.memoryTypeCount; i++ )
                {
                    if( ( type & ( 1 << i ) ) && ( ( physicalDeviceMemoryProperties.memoryTypes[ i ].propertyFlags & properties ) == properties ) ) return i;
                }
                return -1;
            }

            inline void createBuffer( VkDevice device, VkBufferCreateInfo BufferCreateInfo, VkMemoryPropertyFlags mProperties, VkBuffer &Buffer, VkDeviceMemory &mBuffer, VkDeviceSize MemoryOffset, VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties )
            {
                VkResult Result{ vkCreateBuffer( device, &BufferCreateInfo, nullptr, &Buffer ) };
                VkMemoryRequirements Requirements;
                vkGetBufferMemoryRequirements( device, Buffer, &Requirements );
                auto MemoryAllocateInfo{ memoryAllocateInfo() };
                MemoryAllocateInfo.allocationSize  = Requirements.size;
                MemoryAllocateInfo.memoryTypeIndex = memoryTypeIndex( Requirements.memoryTypeBits, mProperties, physicalDeviceMemoryProperties );
                Result                             = vkAllocateMemory( device, &MemoryAllocateInfo, nullptr, &mBuffer );
                Result                             = vkBindBufferMemory( device, Buffer, mBuffer, MemoryOffset );
            }

            inline void createImage( VkDevice device, const uint32_t width, const uint32_t height, const VkBufferUsageFlags iUsage, const VkImageTiling tiling, const VkMemoryPropertyFlags mProperties, VkImageCreateInfo ImageCreateInfo, VkImage &Image, VkDeviceMemory &mImage, VkDeviceSize MemoryOffset, VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties )
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

                auto MemoryAllocateInfo{ memoryAllocateInfo() };
                MemoryAllocateInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                MemoryAllocateInfo.allocationSize  = MemoryRequirements.size;
                MemoryAllocateInfo.memoryTypeIndex = memoryTypeIndex( MemoryRequirements.memoryTypeBits, mProperties, physicalDeviceMemoryProperties );
                Result                             = vkAllocateMemory( device, &MemoryAllocateInfo, nullptr, &mImage );
                Result                             = vkBindImageMemory( device, Image, mImage, MemoryOffset );
            }

            inline VkCommandBuffer beginSingleTimeCommand( VkDevice device, VkCommandPool commandPool )
            {
                VkCommandBuffer commandBuffer;
                VkCommandBufferAllocateInfo CommandBufferAllocateInfo{};
                CommandBufferAllocateInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
                CommandBufferAllocateInfo.commandPool        = commandPool;
                CommandBufferAllocateInfo.commandBufferCount = 1;
                CommandBufferAllocateInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
                VkResult Result{ vkAllocateCommandBuffers( device, &CommandBufferAllocateInfo, &commandBuffer ) };
                VkCommandBufferBeginInfo CommandBufferBeginInfo{};
                CommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
                CommandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
                vkBeginCommandBuffer( commandBuffer, &CommandBufferBeginInfo );
                return commandBuffer;
            }

            inline void endSingleTimeCommand( VkDevice device, VkCommandPool commandPool, VkCommandBuffer commandBuffer, VkQueue queue )
            {
                vkEndCommandBuffer( commandBuffer );
                VkSubmitInfo SubmitInfo{};
                SubmitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                SubmitInfo.commandBufferCount = 1;
                SubmitInfo.pCommandBuffers    = &commandBuffer;
                VkResult Result{ vkQueueSubmit( queue, 1, &SubmitInfo, nullptr ) };
                vkQueueWaitIdle( queue );
                vkFreeCommandBuffers( device, commandPool, 1, &commandBuffer );
            }

            inline void bufcpy( VkCommandBuffer commandBuffer, VkBuffer bSrc, VkBuffer bDst, std::vector<VkBufferCopy> &BufferCopyInfo )
            {
                vkCmdCopyBuffer( commandBuffer, bSrc, bDst, static_cast<uint32_t>( BufferCopyInfo.size() ), BufferCopyInfo.data() );
            }

            inline VkShaderModule loadShader( VkDevice device, const char *cPath )
            {
                std::ifstream File{ cPath, std::fstream::ate | std::fstream::binary };
                if( !File.is_open() )
                {
                    SPDLOG_CRITICAL( "Failed to open shader: {}.", cPath );
                    throw std::runtime_error( std::format( "Failed to open shader: {}.", cPath ) );
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
                VkResult Result{ vkCreateShaderModule( device, &ShaderModuleCreateInfo, nullptr, &shaderModule ) };
                return shaderModule;
            }

        } // namespace tools
    }     // namespace
} // namespace Engine

namespace std
{
    template <>
    struct std::hash<Engine::tools::vertex>
    {
        size_t operator()( Engine::tools::vertex const &vert ) const
        {
            return ( ( hash<glm::vec3>()( vert.coordinate ) ^
                       ( hash<glm::vec3>()( vert.color ) << 1 ) ) >>
                     1 ) ^
                   ( hash<glm::vec2>()( vert.texture ) << 1 );
        }
    };
} // namespace std