#pragma once
#ifndef ENGINE_HXX
#    define ENGINE_HXX
#    define TINYOBJLOADER_IMPLEMENTATION
#    define GLM_FORCE_RADIANS
#    define GLM_FORCE_DEPTH_ZERO_TO_ONE
#    define STB_IMAGE_IMPLEMENTATION
#    define RESOLUTION_TYPE uint16_t
#    define DEFINE_HANDLE( object ) \
        class object;               \
        namespace types             \
        {                           \
            typedef object *object; \
        }
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
    class instance;
    namespace window
    {
        struct ENGINE_EXPORT resolution
        {
            RESOLUTION_TYPE width { 0 };
            RESOLUTION_TYPE height { 0 };
        };

        typedef void ( *ResizeCallback )( int width, int height );
        typedef void ( *KeyEventCallBack )( int key, int scancode, int action, int mods );

        class ENGINE_EXPORT window
        {
          private:
            class data;

          public:
            window( RESOLUTION_TYPE width, RESOLUTION_TYPE height, const char *title, Engine::instance *instance );
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
            const std::unique_ptr<data> data;
            ;
            ~window();
        };
        DEFINE_HANDLE( window );
    } // namespace window

    enum DeviceType
    {
        OTHER          = 0x0ui8,
        CPU            = 0x1ui8,
        VIRTUAL_GPU    = 0x2ui8,
        INTEGRATED_GPU = 0x3ui8,
        DISCRETE_GPU   = 0x4ui8,
    };

    enum ShaderStage
    {
        ALL_SHADER_TYPE,
        VERTEX_SHADER_TYPE,
        FRAGMENT_SHADER_TYPE
    };

    struct ENGINE_EXPORT PipelineInfo
    {
        std::vector<shaderID> shadersID;
    };

    struct ENGINE_EXPORT DeviceDescription
    {
      private:
        class data;

      public:
        const char *name;
        DeviceType type;
        uint32_t grade;
        const std::unique_ptr<data> data;
        ;
    };
    DEFINE_HANDLE( DeviceDescription );
    DEFINE_HANDLE( texture );
    DEFINE_HANDLE( model );

    class ENGINE_EXPORT device
    {
      private:
        class data;

      public:
        device( types::DeviceDescription description );
        types::texture CreateTexture( const char *path );
        types::texture CreateTexture( std::string path );
        types::model CreateModel( const char *path );
        types::model CreateModel( std::string path );
        types::model CreateModel( const char *path, types::texture texture );
        types::model CreateModel( std::string path, types::texture texture );
        const std::unique_ptr<data> data;
        ~device();
    };
    DEFINE_HANDLE( device );

    class ENGINE_EXPORT link
    {
      private:
        class data;

      public:
        link( window::types::window window, types::device device );
        ~link();
        const std::unique_ptr<data> data;
    };

    DEFINE_HANDLE( link );

    class ENGINE_EXPORT instance
    {
      private:
        class data;

      public:
        instance( const char *appName = nullptr, uint32_t appVersion = 0 );
        const std::vector<types::DeviceDescription> GetDevices() const;
        window::types::window CreateWindow( RESOLUTION_TYPE width, RESOLUTION_TYPE height, std::string title );
        window::types::window CreateWindow( RESOLUTION_TYPE width, RESOLUTION_TYPE height, const char *title );
        types::device CreateDevice( types::DeviceDescription description );
        types::link CreateLink( window::types::window window, types::device device );
        const std::unique_ptr<data> data;
        ~instance();
    };
    DEFINE_HANDLE( instance );

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
