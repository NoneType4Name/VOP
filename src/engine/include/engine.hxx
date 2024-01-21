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
#    include <set>
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
    DEFINE_HANDLE( deviceDescription );
    DEFINE_HANDLE( device );
    DEFINE_HANDLE( image );
    DEFINE_HANDLE( buffer );
    DEFINE_HANDLE( commandBuffer );
    DEFINE_HANDLE( swapchain );
    DEFINE_HANDLE( renderPass );

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
        };

        class ENGINE_EXPORT window
        {
            DEFINE_DATA;

          public:
            window() = delete;
            window( instance *instance, settings settings );
            window( bool, Engine::instance *instance, settings settings );
            ~window();
            types::swapchain getLink( types::device device );
            resolution getDisplayResolution();
            void setTitle( const char *title );
            void updateProperties( settings properties );
            void updateEvents();
            bool shouldClose();
            virtual void eventCallBack( int key, int scancode, int action, int mods );
            virtual void resizeCallBack( int width, int height );
            const settings properties {};
            GLFWwindow *glfwHandle;
            VkSurfaceKHR surface;
        };
        DEFINE_HANDLE( window );
    } // namespace window

    struct ENGINE_EXPORT deviceDescription
    {
        DEFINE_DATA;

      public:
        deviceDescription() = delete;
        deviceDescription( instance *instance, VkPhysicalDevice phDevice );
        ~deviceDescription();
        const char *name;
        VkPhysicalDeviceType type;
        uint32_t grade;
    };

    class ENGINE_EXPORT device
    {
        DEFINE_DATA;

      public:
        device() = delete;
        device( types::deviceDescription description, std::vector<window::types::window> windows );
        device( bool, types::deviceDescription description, std::vector<window::types::window> windows );
        ~device();
        VkFormat formatPriority( const std::vector<VkFormat> &formats, VkImageTiling ImageTiling, VkFormatFeatureFlags FormatFeatureFlags );
        uint32_t requeredMemoryTypeIndex( uint32_t type, VkMemoryPropertyFlags properties );
        types::swapchain getLink( window::types::window window );
        class memory
        {
            DEFINE_DATA;

          public:
            memory() = delete;
            memory( types::device device );
            ~memory();
            struct allocationAddres
            {
                VkDeviceSize size;
                VkDeviceSize offset;
                VkDeviceMemory memory;
                uint32_t memoryType;
                size_t memoryBlock;
            };

            allocationAddres allocate( VkImage image, VkMemoryPropertyFlags flags );
            allocationAddres allocate( VkBuffer buffer, VkMemoryPropertyFlags flags );
            void free( allocationAddres &addres );
        };
        memory memory;
        VkCommandPool grapchicPool;
        VkCommandPool transferPool;
        VkCommandPool presentPool;
        VkDevice handle { nullptr };
        // types::shader CreateShader( const char *path, const char *main, ShaderStage stage );
        // types::pipeline CreatePipeline( types::layout layouts, std::vector<types::shader> shaders, types::pass pass );
        // types::texture CreateTexture( const char *path );
        // types::model CreateModel( const char *path, types::texture texture );
    };

    class ENGINE_EXPORT swapchain
    {
        DEFINE_DATA;

      public:
        swapchain() = delete;
        swapchain( types::device device, window::types::window window );
        swapchain( bool, types::device device, window::types::window window );
        ~swapchain();
        struct
        {
            VkSurfaceCapabilitiesKHR capabilities;
            std::vector<VkSurfaceFormatKHR> formats;
            std::vector<VkPresentModeKHR> presentModes;
        } properties;
        struct image_T
        {
            types::image image;
            VkSemaphore available;
            VkSemaphore rendered;
        };
        std::vector<image_T> images;
        VkSurfaceFormatKHR format;
        VkPresentModeKHR presentMode;
        VkSwapchainKHR handle { nullptr };
    };

    class ENGINE_EXPORT image
    {
        DEFINE_DATA;

      public:
        image() = delete;
        image( types::device device, VkImageCreateInfo ImageCreateInfo, VkImageViewCreateInfo ImageViewCreateInfo, VkMemoryPropertyFlags memoryPropertiesFlag );
        ~image();
        image( types::device device, types::image parent, VkImageViewCreateInfo ImageViewCreateInfo );
        void write( std::vector<void *> data, VkExtent3D srcExtend = { 0, 0, 0 }, VkOffset3D srcOffset = { 0, 0, 0 }, VkImageAspectFlags srcAspectMask = VK_IMAGE_ASPECT_COLOR_BIT, uint32_t dstMipLevel = 0, uint32_t arrayLayersCount = 1, uint32_t dstBaseArrayLayer = 0, VkMemoryMapFlags flags = 0 );
        void transition( VkImageLayout newLayout, VkDependencyFlags dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT, VkPipelineStageFlags srcStageMask = 0, VkPipelineStageFlags dstStageMask = 0, uint32_t srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED, uint32_t dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED );
        struct
        {
            VkExtent3D extent;
            uint32_t mipLevels;
            uint32_t arrayLayers;
            VkImageUsageFlags usage;
            VkSampleCountFlagBits samples;
            VkImageType type;
            VkFormat format;
            VkImageLayout layout;
        } properties;
        struct
        {
            VkImageViewType type;
            VkFormat format;
            VkImageSubresourceRange range;
            VkImageView handle { nullptr };
        } view;

        device::memory::allocationAddres addres;
        VkImage handle { nullptr };
    };

    class buffer
    {
        DEFINE_DATA;

      public:
        buffer() = delete;
        buffer( types::device device, VkBufferCreateInfo BufferCreateInfo, VkMemoryPropertyFlags memoryPropertiesFlag );
        ~buffer();
        void write( std::vector<void *> data, VkMemoryMapFlags flags = 0 );
        VkBuffer handle { nullptr };
    };

    class commandBuffer
    {
        DEFINE_DATA;

      public:
        commandBuffer() = delete;
        commandBuffer( types::device device, VkCommandPool commandPool, VkCommandBufferLevel level, struct queue &queue );
        ~commandBuffer();
        void begin();
        void submit();
        VkCommandBuffer handle { nullptr };
    };

    // class renderPass
    // {
    //     DEFINE_DATA;

    //   public:
    //     renderPass() = delete;
    //     renderPass( types::swapchain swapchain, std::vector<types::image> attachments );
    //     renderPass( bool, types::swapchain swapchain, std::vector<types::image> attachments );
    //     ~renderPass();
    //     VkRenderPass handle { nullptr };
    // };

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

      public:
        instance( const char *appName, uint32_t appVersion );
        instance( bool, const char *appName, uint32_t appVersion );
        ~instance();
        virtual window::types::window createWindow( window::settings settings );
        virtual types::device createDevice( types::deviceDescription description, std::vector<window::types::window> windows );
        // virtual types::renderPass createRenderPass();
        // virtual std::pair<types::swapchain, types::device> makeLink( window::types::window window, types::deviceDescription description );
        // ? virtual types::pass createRenderPass( types::swapchain swapchain );
        const std::vector<types::deviceDescription> &getDevices();
        VkInstance handle { nullptr };
    };
} // namespace Engine

#endif
