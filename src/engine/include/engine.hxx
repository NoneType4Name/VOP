#ifndef ENGINE_HXX
#define ENGINE_HXX

#define TINYOBJLOADER_IMPLEMENTATION
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define STB_IMAGE_IMPLEMENTATION
#define GLFW_INCLUDE_VULKAN
#if defined( _WIN32 )
#    define VK_USE_PLATFORM_WIN32_KHR
#    define GLFW_EXPOSE_NATIVE_WIN32
// #elif defined( __LINUX__ )
// #    define VK_USE_PLATFORM_X11_KHR
// #    define GLFW_EXPOSE_NATIVE_X11
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
#include <string>
#include <stdint.h>
#include "engine_export.hxx"

namespace Engine
{
    typedef void ( *KeyEventCallBack )( int key, int scancode, int action, int mods );
    enum GrapchiDeviceType
    {
        OTHER = 0,
        INTEGRATED_GPU,
        DISCRETE_GPU,
        VIRTUAL_GPU,
        CPU
    };

    struct ENGINE_EXPORT GrapchicPhysicalDevice
    {
        const char *name;
        GrapchiDeviceType type;
        uint16_t rating{ 0 };
        uint32_t deviceID;
    };

    ENGINE_EXPORT void WindowInit( uint16_t width, uint16_t height, const char *title );
    ENGINE_EXPORT GrapchicPhysicalDevice *SetGraphicDevice( GrapchicPhysicalDevice device );
    ENGINE_EXPORT void WindowDestroy();
    ENGINE_EXPORT void SetWindowResolution( uint16_t width, uint16_t height );
    ENGINE_EXPORT void CentralizeWindow();
    ENGINE_EXPORT void SetWindowTitle( const char *new_title );
    ENGINE_EXPORT void SetWindowTitle( std::string &new_title );
    ENGINE_EXPORT KeyEventCallBack SetKeyEventsCallback( KeyEventCallBack Callback );
    ENGINE_EXPORT bool WindowShouldClose();
    ENGINE_EXPORT void UpdateEvents();

} // namespace Engine

#endif
