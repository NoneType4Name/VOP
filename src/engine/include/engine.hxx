#ifndef RENDER_HXX
#define RENDER_HXX

#define TINYOBJLOADER_IMPLEMENTATION
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define STB_IMAGE_IMPLEMENTATION
#define GLFW_INCLUDE_VULKAN
#if defined( _WIN32 )
#    define VK_USE_PLATFORM_WIN32_KHR
#    define GLFW_EXPOSE_NATIVE_WIN32
#elif defined( __LINUX__ )
#    define VK_USE_PLATFORM_X11_KHR
#    define GLFW_EXPOSE_NATIVE_X11
#endif
#ifndef DEBUG
#    ifdef _DEBUG
#        define DEBUG               true
#        define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#    else
#        define DEBUG               false
#        define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_CRITICAL
#    endif
#endif

#include <spdlog/spdlog.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <glm/gtx/hash.hpp>
#include <tiny_obj_loader.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <glm/gtc/matrix_transform.hpp>
#include <vulkan/vk_enum_string_helper.h>
#include <array>
#include <vector>
#include <format>
#include <set>
#include <iostream>
#include "engine_export.hxx"

namespace Engine
{
    struct ENGINE_EXPORT QueueFamilyIndices
    {
        std::optional<uint32_t> graphic;
        std::optional<uint32_t> present;
        std::optional<uint32_t> transfer;

        bool isComplete()
        {
            return graphic.has_value() && present.has_value() && transfer.has_value();
        }
    };

    struct ENGINE_EXPORT SwapChain
    {
        VkSwapchainKHR Swapchain;
        VkSurfaceCapabilitiesKHR Capabilitie;
        VkFormat Format;
        VkPresentModeKHR PresentMode;
        std::vector<VkSurfaceFormatKHR> AviliableFormat;
        std::vector<VkPresentModeKHR> AviliablePresentModes;
    };

    struct ENGINE_EXPORT Vertex
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

    struct ENGINE_EXPORT Model
    {
        std::vector<Vertex> ModelVertecies;
        std::vector<uint32_t> ModelVerteciesIndices;
        uint32_t VerteciesOffset{};
        uint32_t IndeciesOffset{};
    };

    struct ENGINE_EXPORT DemensionsUniformrObject
    {
        alignas( 16 ) glm::mat4 model;
        alignas( 16 ) glm::mat4 view;
        alignas( 16 ) glm::mat4 proj;
    };
    void ENGINE_EXPORT WindowInit( uint16_t width, uint16_t height, const char *title );
    void ENGINE_EXPORT WindowDestroy();
    void ENGINE_EXPORT SetWindowResolution( uint16_t width, uint16_t height );
    void ENGINE_EXPORT CentralizeWindow();
    void ENGINE_EXPORT SetWindowTitle( const char *new_title );
    void ENGINE_EXPORT SetWindowTitle( std::string &new_title );
    void ENGINE_EXPORT GetEvents();

} // namespace Engine

#endif