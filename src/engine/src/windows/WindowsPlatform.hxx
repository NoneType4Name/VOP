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
    namespace
    {
        namespace tools
        {
            void createSurface( VkInstance instance );
            const VkSurfaceKHR getSurface();

        } // namespace tools
    }     // namespace
    namespace window
    {
        struct resolution
        {
            RESOLUTION_TYPE width{ 0 };
            RESOLUTION_TYPE height{ 0 };
        };
        typedef void ( *ResizeCallback )( int width, int height );
        typedef void ( *EventCallBack )( int key, int scancode, int action, int mods );
        resolution getResolution();
        resolution getDisplayResolution();
        void create();
        void cenralize();
        void setTitle( const char *title );
        void setTitle( std::string title );
        void setWindowResolution( RESOLUTION_TYPE width, RESOLUTION_TYPE height );
        void setResizeCallBack( ResizeCallback callback );
        void setKeyEventsCallback( EventCallBack callback );
        void updateEvents();
        bool shouldClose();
    } // namespace window
} // namespace Engine