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
#    define DEFINE_DATA                \
      public:                          \
        class ENGINE_EXPORT DATA_TYPE; \
        const std::unique_ptr<DATA_TYPE> data;
#    include <array>
#    include <memory>
#    include <vector>
#    include <string>
#    include <stdint.h>
#    include <glm/glm.hpp>
#    include <glm/gtx/hash.hpp>
#    include "engine_export.hxx"
#    include <glm/gtc/matrix_transform.hpp>
#    include <platform.hxx>

namespace Engine
{
    DEFINE_HANDLE( instance );
    DEFINE_HANDLE( DeviceDescription );
    DEFINE_HANDLE( device );
    DEFINE_HANDLE( swapchain );
    DEFINE_HANDLE( image );

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
            const char *title;
            int fullScreenRefreshRate { 0 };
            bool resize { true };
            bool decorated { true };
            bool floating { false };
            bool visible { true };
            // bool vsync { false };
        };

        class ENGINE_EXPORT window
        {
            DEFINE_DATA;

          protected:
            window( Engine::instance *instance, settings settings );
            virtual void setup();
            virtual void eventCallBack( int key, int scancode, int action, int mods );
            virtual void resizeCallBack( int width, int height );
            friend Engine::instance;

          public:
            window();
            resolution getDisplayResolution();
            void setTitle( const char *title );
            void updateProperties( settings properties );
            void updateEvents();
            bool shouldClose();
            ~window();
            const settings properties {};
        };
        DEFINE_HANDLE( window );
    } // namespace window

    // DEFINE_HANDLE( pass );
    // DEFINE_HANDLE( shader );
    // DEFINE_HANDLE( layout );
    // DEFINE_HANDLE( pipeline );
    // DEFINE_HANDLE( texture );
    // DEFINE_HANDLE( buffer );
    // DEFINE_HANDLE( model );

    struct ENGINE_EXPORT DeviceDescription
    {
        DEFINE_DATA;

      private:
        friend struct queue;

      public:
        DeviceDescription();
        const char *name;
        VkPhysicalDeviceType type;
        uint32_t grade;
        ~DeviceDescription();
    };

    class ENGINE_EXPORT device
    {
        DEFINE_DATA;

      private:
        device( types::DeviceDescription description );
        virtual void setup();
        virtual void setup( window::types::window window );
        friend instance;

      public:
        device();
        virtual types::swapchain bindWindow( window::types::window window );
        // types::shader CreateShader( const char *path, const char *main, ShaderStage stage );
        // types::pipeline CreatePipeline( types::layout layouts, std::vector<types::shader> shaders, types::pass pass );
        // types::texture CreateTexture( const char *path );
        // types::model CreateModel( const char *path, types::texture texture );
        ~device();
    };

    class ENGINE_EXPORT image
    {
        DEFINE_DATA;

      private:
        image( types::device device, VkExtent3D extend, const VkBufferUsageFlags usage, const VkImageTiling tiling, const VkMemoryPropertyFlags mProperties, VkImageAspectFlags aspect, VkFormat format, VkImageCreateInfo ImageCreateInfo = {} );
        image( types::device device, VkExtent3D extend, VkImage image, VkImageAspectFlags aspect, VkFormat format, uint32_t mipLevels, uint32_t arrayLayers );
        friend device;
    };

    class ENGINE_EXPORT swapchain
    {
        DEFINE_DATA;

      private:
        swapchain( types::device device, window::types::window window );
        virtual void setup();
        friend device;

      public:
        swapchain();
        ~swapchain();
    };

    // class ENGINE_EXPORT shader
    // {
    //   private:
    //     class ENGINE_EXPORT DATA_TYPE;
    //     shader( types::device device, const char *path, const char *mainFuncName, ShaderStage stage );
    //     friend device;

    //   public:
    //     shader();
    //     ~shader();
    //     DATA_PTR data;
    // };

    // class ENGINE_EXPORT layout
    // {
    //   private:
    //     class ENGINE_EXPORT DATA_TYPE;
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
    //     DATA_PTR data;
    // };

    // class ENGINE_EXPORT descriptorPool
    // {
    //   private:
    //     class ENGINE_EXPORT DATA_TYPE;
    //     descriptorPool( types::device device, void *userData );
    //     friend device;

    //   public:
    //     descriptorPool();
    //     ~descriptorPool();
    //     struct descriptorSetLayoutInfo;
    //     typedef std::vector<std::vector<descriptorSetLayoutInfo>> SetOfBindingsInfo;
    //     DATA_PTR data;
    // };

    // class ENGINE_EXPORT pipeline
    // {
    //   private:
    //     class ENGINE_EXPORT DATA_TYPE;
    //     pipeline( types::device device, types::modelsPool pool, std::vector<types::shader> shaders, types::pass pass );
    //     friend device;

    //   public:
    //     pipeline();
    //     ~pipeline();
    //     DATA_PTR data;
    // };

    // class ENGINE_EXPORT pass
    // {
    //   private:
    //     pass( Engine::types::swapchain swapchain );
    //     class ENGINE_EXPORT DATA_TYPE;
    //     friend instance;

    //   public:
    //     pass();
    //     ~pass();

    //     DATA_PTR data;
    // };

    // class ENGINE_EXPORT model
    // {
    //   private:
    //     model( types::instance device );
    //     class ENGINE_EXPORT DATA_TYPE;

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
        DEFINE_DATA;

      protected:
        virtual void setup( const char *appName, uint32_t appVersion );

      public:
        instance();
        void init( const char *appName, uint32_t appVersion );
        virtual window::types::window createWindow( ENGINE_RESOLUTION_TYPE width, ENGINE_RESOLUTION_TYPE height, const char *title, int fullScreenRefreshRate = 0, bool resize = 0 );
        virtual types::device createDevice( types::DeviceDescription description );
        // virtual std::pair<types::swapchain, types::device> makeLink( window::types::window window, types::DeviceDescription description );
        // ? virtual types::pass createRenderPass( types::swapchain swapchain );
        const std::vector<types::DeviceDescription> &getDevices();
        ~instance();
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
