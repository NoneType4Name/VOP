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
#    define DEFINE_TEMPLATE( name )             \
        class name : Engine::render::attachment \
        {                                       \
          public:                               \
            name( types::link link );           \
        };
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
            class DATA_TYPE;
            window( RESOLUTION_TYPE width, RESOLUTION_TYPE height, const char *title, Engine::instance *instance );
            friend class Engine::instance;

          public:
            window();
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
            const std::unique_ptr<DATA_TYPE> data;
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

    struct ENGINE_EXPORT DeviceDescription
    {
      private:
        class DATA_TYPE;
        friend struct queue;

      public:
        DeviceDescription();
        const char *name;
        DeviceType type;
        uint32_t grade;
        const std::unique_ptr<DATA_TYPE> data;
        ~DeviceDescription();
    };
    DEFINE_HANDLE( DeviceDescription );
    DEFINE_HANDLE( texture );
    DEFINE_HANDLE( model );

    class ENGINE_EXPORT device
    {
      private:
        class DATA_TYPE;
        device( types::DeviceDescription description, window::types::window window );
        friend class instance;

      public:
        device();
        types::texture CreateTexture( const char *path );
        types::texture CreateTexture( std::string path );
        types::model CreateModel( const char *path );
        types::model CreateModel( std::string path );
        types::model CreateModel( const char *path, types::texture texture );
        types::model CreateModel( std::string path, types::texture texture );
        const std::unique_ptr<DATA_TYPE> data;
        ~device();
    };
    DEFINE_HANDLE( device );

    class ENGINE_EXPORT link
    {
      private:
        class DATA_TYPE;
        friend class instance;

      public:
        link();
        ~link();
        const std::unique_ptr<DATA_TYPE> data;
    };
    DEFINE_HANDLE( link );

    namespace render
    {
        class ENGINE_EXPORT attachment
        {
          private:
            class DATA_TYPE;

          public:
            attachment();
            attachment( types::link link );
            ~attachment();
            const std::unique_ptr<DATA_TYPE> data;
        };
        DEFINE_HANDLE( attachment );

        class ENGINE_EXPORT subpass
        {
          private:
            class DATA_TYPE;

          public:
            subpass();
            virtual void setup();
            ~subpass();

            const std::unique_ptr<DATA_TYPE> data;
        };

        DEFINE_HANDLE( subpass );

        class ENGINE_EXPORT subpassLink
        {
          private:
            class DATA_TYPE;

          public:
            subpassLink();
            ~subpassLink();

            const std::unique_ptr<DATA_TYPE> data;
        };
        DEFINE_HANDLE( subpassLink );

        class ENGINE_EXPORT pass
        {
          private:
            class DATA_TYPE;

          public:
            pass();
            ~pass();

            const std::unique_ptr<DATA_TYPE> data;
        };
        DEFINE_HANDLE( pass );
    } // namespace render

    class ENGINE_EXPORT instance
    {
      private:
        class DATA_TYPE;

      public:
        instance();
        instance( const char *appName, uint32_t appVersion = 0 );
        const std::vector<types::DeviceDescription> GetDevices();
        window::types::window createWindow( RESOLUTION_TYPE width, RESOLUTION_TYPE height, std::string title );
        window::types::window createWindow( RESOLUTION_TYPE width, RESOLUTION_TYPE height, const char *title );
        types::link CreateLink( window::types::window window, types::DeviceDescription description );
        render::types::pass CreateRenderPass( render::types::attachment subpassesLink, types::link link );
        ~instance();

        const std::unique_ptr<DATA_TYPE> data;
    };
    DEFINE_HANDLE( instance )

    // ENGINE_EXPORT shaderID CreateShader( const char *path, const char *mainFuncName, ShaderStage stage );
    // // ENGINE_EXPORT pipelineID CreateDescriptorSet( DescriptorSetInfo info );
    // ENGINE_EXPORT pipelineID CreatePipeline( PipelineInfo info );
    // namespace templates
    // {
    //     DEFINE_TEMPLATE( colorAttachment );
    //     DEFINE_TEMPLATE( colorResolveAttachment );
    //     DEFINE_TEMPLATE( depthAttachment ); // setup only in subpass?.
    // } // namespace templates
} // namespace Engine

#endif
