#pragma once
#ifndef ENGINE_HXX
#    define ENGINE_HXX
#    define TINYOBJLOADER_IMPLEMENTATION
#    define GLM_FORCE_RADIANS
#    define GLM_FORCE_DEPTH_ZERO_TO_ONE
#    define STB_IMAGE_IMPLEMENTATION
#    define RESOLUTION_TYPE uint16_t

#    include <vector>
#    include <array>
#    include <string>
#    include <stdint.h>
#    include "engine_export.hxx"

namespace Engine
{
    namespace window
    {
        struct ENGINE_EXPORT resolution
        {
            RESOLUTION_TYPE width{ 0 };
            RESOLUTION_TYPE height{ 0 };
        };
        typedef void ( *ResizeCallback )( int width, int height );
        typedef void ( *KeyEventCallBack )( int key, int scancode, int action, int mods );
        ENGINE_EXPORT resolution getResolution();
        ENGINE_EXPORT resolution getDisplayResolution();
        ENGINE_EXPORT void cenralize();
        ENGINE_EXPORT void setTitle( const char *title );
        ENGINE_EXPORT void setTitle( std::string title );
        ENGINE_EXPORT void setWindowResolution( RESOLUTION_TYPE width, RESOLUTION_TYPE height );
        ENGINE_EXPORT void setResizeCallBack( ResizeCallback callback );
        ENGINE_EXPORT void setKeyEventsCallback( KeyEventCallBack callback );
        ENGINE_EXPORT void updateEvents();
        ENGINE_EXPORT bool shouldClose();
    } // namespace window

    enum PhysicalDeviceType
    {
        OTHER          = 0x0ui8,
        INTEGRATED_GPU = 0x1ui8,
        DISCRETE_GPU   = 0x2ui8,
        VIRTUAL_GPU    = 0x4ui8,
        CPU            = 0x8ui8
    };

    enum MultiSamplingCount
    {
        SAMPLE_COUNT_1_BIT  = 0x01,
        SAMPLE_COUNT_2_BIT  = 0x02,
        SAMPLE_COUNT_4_BIT  = 0x04,
        SAMPLE_COUNT_8_BIT  = 0x08,
        SAMPLE_COUNT_16_BIT = 0x10,
        SAMPLE_COUNT_32_BIT = 0x20,
        SAMPLE_COUNT_64_BIT = 0x40,
    };

    typedef uint64_t textureID;
    typedef uint64_t modelID;
    enum modelType
    {
        MODEL_TYPE_GUI    = 1,
        MODEL_TYPE_ENTITY = 2
    };

    struct ENGINE_EXPORT Device
    {
        const char *name{ nullptr };
        void *ptr;
    };

    struct ENGINE_EXPORT Settings
    {
        MultiSamplingCount MultiSamplingCount;
    };

    struct ENGINE_EXPORT AppCreateInfo
    {
        uint16_t width{ 800 };
        uint16_t height{ 600 };
        const char *title{};
        Device device;
        const char *VertexShaderPath;
        const char *FragmentShaderPath;
        Settings settings{};
    };

    ENGINE_EXPORT void init( AppCreateInfo sAppCreateInfo );
    ENGINE_EXPORT void shutdown();
    ENGINE_EXPORT std::vector<Device> GetGraphicDevices( uint8_t devicesTypeFlag = 0 );
    ENGINE_EXPORT void SetGraphicDevice( Device device ); // todo
    ENGINE_EXPORT Device GetActiveGrapchiDevice();
    ENGINE_EXPORT textureID CreateTexture( const char *path );
    ENGINE_EXPORT modelID CreateModel( modelType, const char *path );
    ENGINE_EXPORT void ModelBindTexture( modelID model, textureID texture );
} // namespace Engine

#endif
