#pragma once
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#define GLFW_EXPOSE_NATIVE_WIN32
#define RESOLUTION_TYPE uint16_t

// #include <vulkan/vulkan.hpp>
#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

namespace Engine
{
    namespace tools
    {
        void createSurface( VkInstance instance );
        void createWindow( RESOLUTION_TYPE width, RESOLUTION_TYPE height, const char *title );
        void destroyWindow();
        const VkSurfaceKHR getSurface();
    } // namespace tools
} // namespace Engine
