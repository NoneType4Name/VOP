#pragma once
#ifndef ENGINE_HXX
#    define ENGINE_HXX
#    ifndef ENGINE_RESOLUTION_TYPE
#        define ENGINE_RESOLUTION_TYPE uint16_t
#    endif
#    define TINYOBJLOADER_IMPLEMENTATION
#    define GLM_FORCE_RADIANS
#    define GLM_FORCE_DEPTH_ZERO_TO_ONE
#    define STB_IMAGE_IMPLEMENTATION
#    define DEFINE_HANDLE( object ) \
        class ENGINE_EXPORT object; \
        namespace types             \
        {                           \
            typedef object *object; \
        }
#    define DATA_TYPE data_T
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
            ENGINE_RESOLUTION_TYPE width { 800 };
            ENGINE_RESOLUTION_TYPE height { 600 };
        };

        struct ENGINE_EXPORT settings
        {
            resolution size;
            std::string title;
            bool fullScreen { false };
            // bool vsync { false };
        };

        typedef void ( *ResizeCallback )( int width, int height );
        typedef void ( *KeyEventCallBack )( int key, int scancode, int action, int mods );

        class ENGINE_EXPORT window
        {
          protected:
            class DATA_TYPE;
            window( Engine::instance *instance, settings settings );
            virtual void setup( settings settings );
            friend Engine::instance;

          public:
            window() {}
            resolution getDisplayResolution();
            void setTitle( const char *title );
            void setWindowResolution( ENGINE_RESOLUTION_TYPE width, ENGINE_RESOLUTION_TYPE height );
            // void setResizeCallBack( ResizeCallback callback );
            // void setKeyEventsCallback( KeyEventCallBack callback );
            void updateEvents();
            bool shouldClose();
            const std::unique_ptr<DATA_TYPE> data;
            ~window();
            resolution size;
            const settings settings;
            const std::string title;
        };
        DEFINE_HANDLE( window );
    } // namespace window

    // enum DeviceType
    // {
    //     OTHER          = 0x0ui8,
    //     CPU            = 0x1ui8,
    //     VIRTUAL_GPU    = 0x2ui8,
    //     INTEGRATED_GPU = 0x3ui8,
    //     DISCRETE_GPU   = 0x4ui8,
    // };

    // enum ShaderStage
    // {
    //     ALL_SHADER_TYPE,
    //     VERTEX_SHADER_TYPE,
    //     FRAGMENT_SHADER_TYPE
    // };
    // typedef uint32_t ShaderStageFlags;

    DEFINE_HANDLE( instance )
    DEFINE_HANDLE( DeviceDescription );
    // DEFINE_HANDLE( device );
    // DEFINE_HANDLE( link );
    // DEFINE_HANDLE( pass );
    // DEFINE_HANDLE( shader );
    // DEFINE_HANDLE( layout );
    // DEFINE_HANDLE( pipeline );
    // DEFINE_HANDLE( texture );
    // DEFINE_HANDLE( buffer );
    // DEFINE_HANDLE( model );

    // struct ENGINE_EXPORT DeviceDescription
    // {
    //   private:
    //     class DATA_TYPE;
    //     friend struct queue;

    //   public:
    //     DeviceDescription();
    //     const char *name;
    //     DeviceType type;
    //     uint32_t grade;
    //     const std::unique_ptr<DATA_TYPE> data;
    //     ~DeviceDescription();
    // };

    // class ENGINE_EXPORT device
    // {
    //   private:
    //     class DATA_TYPE;
    //     device( types::DeviceDescription description, window::types::window window );
    //     friend class instance;

    //   public:
    //     device();
    //     types::shader CreateShader( std::string path, std::string main, ShaderStage stage );
    //     types::shader CreateShader( const char *path, const char *main, ShaderStage stage );
    //     types::pipeline CreatePipeline( types::layout layouts, std::vector<types::shader> shaders, types::pass pass );
    //     types::texture CreateTexture( const char *path );
    //     types::texture CreateTexture( std::string path );
    //     types::model CreateModel( const char *path, types::texture texture );
    //     types::model CreateModel( std::string path, types::texture texture );
    //     const std::unique_ptr<DATA_TYPE> data;
    //     ~device();
    // };

    // class ENGINE_EXPORT link
    // {
    //   private:
    //     class DATA_TYPE;
    //     link( window::types::window window, types::device device );
    //     friend instance;

    //   public:
    //     link();
    //     ~link();
    //     const std::unique_ptr<DATA_TYPE> data;
    // };

    // class ENGINE_EXPORT shader
    // {
    //   private:
    //     class DATA_TYPE;
    //     shader( types::device device, const char *path, const char *mainFuncName, ShaderStage stage );
    //     friend device;

    //   public:
    //     shader();
    //     ~shader();
    //     const std::unique_ptr<DATA_TYPE> data;
    // };

    // class ENGINE_EXPORT layout
    // {
    //   private:
    //     class DATA_TYPE;
    //     layout( types::device device, types::descriptorPool pool, void *userData );
    //     friend device;

    //   public:
    //     struct pushConstantRange
    //     {
    //         uint32_t offset;
    //         uint32_t size;
    //         ShaderStageFlags stage;
    //     };

    //     layout();
    //     ~layout();
    //     const std::unique_ptr<DATA_TYPE> data;
    // };

    // class ENGINE_EXPORT descriptorPool
    // {
    //   private:
    //     class DATA_TYPE;
    //     descriptorPool( types::device device, void *userData );
    //     friend device;

    //   public:
    //     descriptorPool();
    //     ~descriptorPool();
    //     struct descriptorSetLayoutInfo;
    //     typedef std::vector<std::vector<descriptorSetLayoutInfo>> SetOfBindingsInfo;
    //     const std::unique_ptr<DATA_TYPE> data;
    // };

    // class ENGINE_EXPORT pipeline
    // {
    //   private:
    //     class DATA_TYPE;
    //     pipeline( types::device device, types::modelsPool pool, std::vector<types::shader> shaders, types::pass pass );
    //     friend device;

    //   public:
    //     pipeline();
    //     ~pipeline();
    //     const std::unique_ptr<DATA_TYPE> data;
    // };

    // class ENGINE_EXPORT pass
    // {
    //   private:
    //     pass( Engine::types::link link );
    //     class DATA_TYPE;
    //     friend instance;

    //   public:
    //     pass();
    //     ~pass();

    //     const std::unique_ptr<DATA_TYPE> data;
    // };

    // class ENGINE_EXPORT model
    // {
    //   private:
    //     model( types::instance device );
    //     class DATA_TYPE;

    //   public:
    //     model( const char *path );
    //     void draw( where );
    //     types::texture getTexture() const;
    //     types::texture bindTexture( types::texture texture );
    //     types::buffer getBuffer() const;
    //     types::buffer bindBuffer( types::buffer buffer );
    //     void setPosition( glm::vec3 positionVector );
    //     void setScale( glm::mat4 scaleMatrix );
    //     void setRotation( glm::mat4 rotateMatrix );
    //     ~model();
    // };

    class ENGINE_EXPORT instance
    {
      protected:
        class ENGINE_EXPORT DATA_TYPE;
        virtual void setup( const char *appName, uint32_t appVersion );

      public:
        instance();
        void init( const char *appName, uint32_t appVersion );
        virtual window::types::window createWindow( ENGINE_RESOLUTION_TYPE width, ENGINE_RESOLUTION_TYPE height, const char *title );
        // virtual std::pair<types::link, types::device> makeLink( window::types::window window, types::DeviceDescription description );
        // ? virtual types::pass createRenderPass( types::link link );
        // const std::vector<types::DeviceDescription> getDevices();
        ~instance();

        const std::unique_ptr<DATA_TYPE> data;
    };
    // DEFINE_RENDER_ATTACHMENT_TEMPLATE( RENDER_ATTACHMENT_DEFAULT_COLOR )
    // DEFINE_RENDER_ATTACHMENT_TEMPLATE( RENDER_ATTACHMENT_DEFAULT_DEPTH )
    // DEFINE_RENDER_ATTACHMENT_TEMPLATE( RENDER_ATTACHMENT_DEFAULT_COLOR_RESOLVE )

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
