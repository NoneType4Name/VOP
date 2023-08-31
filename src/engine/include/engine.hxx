#pragma once
#ifndef ENGINE_HXX
#    define ENGINE_HXX
#    define ENGINE_DEBUG

#    define TINYOBJLOADER_IMPLEMENTATION
#    define GLM_FORCE_RADIANS
#    define GLM_FORCE_DEPTH_ZERO_TO_ONE
#    define STB_IMAGE_IMPLEMENTATION
#    define GLFW_INCLUDE_VULKAN
#    if defined( _WIN32 )
#        define VK_USE_PLATFORM_WIN32_KHR
#        define GLFW_EXPOSE_NATIVE_WIN32
// #elif defined( __LINUX__ )
// #    define VK_USE_PLATFORM_X11_KHR
// #    define GLFW_EXPOSE_NATIVE_X11
#    endif
#    ifndef DEBUG
#        ifdef _DEBUG
#            define DEBUG               true
#            define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#        else
#            define DEBUG               false
#            define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_CRITICAL
#        endif
#    endif
#    include <set>
#    include <array>
#    include <string>
#    include <vector>
#    include <stdint.h>
#    include "engine_export.hxx"

namespace Engine
{
    typedef void ( *KeyEventCallBack )( int key, int scancode, int action, int mods );
    typedef void ( *SettingCallBack )( void *data );

    enum GrapchiDeviceType
    {
        OTHER = 0,
        INTEGRATED_GPU,
        DISCRETE_GPU,
        VIRTUAL_GPU,
        CPU
    };

    struct ENGINE_EXPORT GrapchicPhysicalDeviceSettings
    {
        uint32_t SampleCout{ 0 };
    };

    struct ENGINE_EXPORT GrapchicPhysicalDevice
    {
        const char *name;
        GrapchiDeviceType type;
        uint32_t deviceID;
        GrapchicPhysicalDeviceSettings MaxSettings{};
    };

    struct ENGINE_EXPORT Settings
    {
        uint32_t MultiSamplingCount;
    };

    struct ENGINE_EXPORT AppCreateInfo
    {
        uint16_t width{ 800 };
        uint16_t height{ 600 };
        const char *title{};
        std::vector<const char *> vAppModels{};
        const char *VertexShaderPath;
        const char *FragmentShaderPath;
        Settings sSettings{};
    };

    ENGINE_EXPORT void init( AppCreateInfo sAppCreateInfo );
    ENGINE_EXPORT void SetGraphicDevice( GrapchicPhysicalDevice device );
    ENGINE_EXPORT GrapchicPhysicalDevice GetActiveGrapchiDevice();
    ENGINE_EXPORT std::vector<GrapchicPhysicalDevice> GetGraphicDevices();
    ENGINE_EXPORT void shutdown();
    ENGINE_EXPORT void SetWindowResolution( uint16_t width, uint16_t height );
    ENGINE_EXPORT void CentralizeWindow();
    ENGINE_EXPORT void SetWindowTitle( const char *new_title );
    ENGINE_EXPORT void SetWindowTitle( std::string &new_title );
    ENGINE_EXPORT KeyEventCallBack SetKeyEventsCallback( KeyEventCallBack Callback );
    ENGINE_EXPORT bool WindowShouldClose();
    ENGINE_EXPORT void UpdateEvents();

} // namespace Engine

#endif