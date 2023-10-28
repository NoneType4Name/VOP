#pragma once
#ifndef ENGINE_HXX
#    define ENGINE_HXX
#    define TINYOBJLOADER_IMPLEMENTATION
#    define GLM_FORCE_RADIANS
#    define GLM_FORCE_DEPTH_ZERO_TO_ONE
#    define STB_IMAGE_IMPLEMENTATION
#    define RESOLUTION_TYPE uint16_t
#    include <array>
#    include <memory>
#    include <vector>
#    include <string>
#    include <stdint.h>
#    include <glm/glm.hpp>
#    include <glm/gtx/hash.hpp>
#    include "engine_export.hxx"
#    include <glm/gtc/matrix_transform.hpp>

namespace Engine
{
    namespace window
    {
        struct ENGINE_EXPORT resolution
        {
            RESOLUTION_TYPE width { 0 };
            RESOLUTION_TYPE height { 0 };
        };

        typedef void ( *ResizeCallback )( int width, int height );
        typedef void ( *KeyEventCallBack )( int key, int scancode, int action, int mods );

        class window
        {
          public:
            window() = default;
            window( RESOLUTION_TYPE width, RESOLUTION_TYPE height, const char *title );
            resolution getResolution();
            resolution getDisplayResolution();
            void cenralize();
            void setTitle( const char *title );
            void setTitle( std::string title );
            void setWindowResolution( RESOLUTION_TYPE width, RESOLUTION_TYPE height );
            void setResizeCallBack( ResizeCallback callback );
            void setKeyEventsCallback( KeyEventCallBack callback );
            void updateEvents();
            bool shouldClose();
            ~window();

          private:
            class data;
            std::unique_ptr<data> data;
        };

    } // namespace window

    enum DeviceType
    {
        OTHER          = 0x0ui8,
        INTEGRATED_GPU = 0x1ui8,
        DISCRETE_GPU   = 0x2ui8,
        VIRTUAL_GPU    = 0x4ui8,
        CPU            = 0x8ui8
    };

    struct DeviceDescription
    {
      private:
        class data;

      public:
        const char *name;
        DeviceType type;
        uint32_t grade;
        std::unique_ptr<data> data;
    };

    typedef uint64_t deviceID;
    typedef uint64_t textureID;
    typedef uint64_t modelID;
    typedef uint64_t shaderID;
    typedef uint64_t pipelineID;

    enum ShaderStage
    {
        NONE_SHADER_TYPE,
        VERTEX_SHADER_TYPE,
        FRAGMENT_SHADER_TYPE
    };

    struct ENGINE_EXPORT PipelineInfo
    {
        std::vector<shaderID> shadersID;
    };

    class ENGINE_EXPORT Instance
    {
      private:
        class data;

      public:
        Instance( const char *appName = nullptr, uint32_t appVersion = 0 );
        ~Instance();
        virtual void setupExtensions( std::vector<const char *> &rExtensions );
        virtual void setupLayers( std::vector<const char *> &rLayers );
        std::unique_ptr<data> data;
    };

    // ENGINE_EXPORT void init( AppCreateInfo sAppCreateInfo );
    // ENGINE_EXPORT void shutdown();
    // ENGINE_EXPORT std::vector<Device> GetGraphicDevices( uint8_t devicesTypeFlag = 0 );
    // ENGINE_EXPORT void SetGraphicDevice( Device device ); // todo
    // ENGINE_EXPORT Device GetActiveGrapchicDevice();
    ENGINE_EXPORT textureID CreateTexture( const char *path );
    ENGINE_EXPORT modelID CreateModel( const char *path );
    ENGINE_EXPORT shaderID CreateShader( const char *path, const char *mainFuncName, ShaderStage stage );
    // ENGINE_EXPORT pipelineID CreateDescriptorSet( DescriptorSetInfo info );
    ENGINE_EXPORT pipelineID CreatePipeline( PipelineInfo info );
    ENGINE_EXPORT void ModelBindTexture( modelID model, textureID texture );

} // namespace Engine

#endif
