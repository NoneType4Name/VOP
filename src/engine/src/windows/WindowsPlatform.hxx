#pragma once
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#define GLFW_EXPOSE_NATIVE_WIN32
#define RESOLUTION_TYPE uint16_t

// #include <vulkan/vulkan.hpp>
#include <stddef.h>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3native.h>
#include <vulkan/vk_enum_string_helper.h>

namespace Engine
{
    namespace tools
    {
        void createSurface( VkInstance instance );
        void createWindow( RESOLUTION_TYPE width, RESOLUTION_TYPE height, const char *title );
        void destroySurface();
        void destroyWindow();
        const VkSurfaceKHR getSurface();
        GLFWwindow *getWindow();
    } // namespace tools
} // namespace Engine
