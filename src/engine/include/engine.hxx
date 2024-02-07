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

#    include <glm/glm.hpp>
#    include <glm/gtx/hash.hpp>
#    include <platform.hxx>
#    include <common/globals.hxx>
#    include "engine_export.hxx"
#    include <glm/gtc/matrix_transform.hpp>

namespace Engine
{
    DEFINE_HANDLE( instance );
    DEFINE_HANDLE( deviceDescription );
    DEFINE_HANDLE( device );
    DEFINE_HANDLE( image );
    DEFINE_HANDLE( queue );
    DEFINE_HANDLE( buffer );
    DEFINE_HANDLE( commandPool );
    DEFINE_HANDLE( commandBuffer );
    DEFINE_HANDLE( swapchain );
    DEFINE_HANDLE( renderPass );
    DEFINE_HANDLE( framebuffer );
    DEFINE_HANDLE( descriptorSetLayout );
    DEFINE_HANDLE( descriptorSet );
    DEFINE_HANDLE( descriptorsPool );

    namespace window
    {
        struct ENGINE_EXPORT resolution final
        {
            ENGINE_RESOLUTION_TYPE width { 800 };
            ENGINE_RESOLUTION_TYPE height { 600 };
        };

        struct ENGINE_EXPORT settings final
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

    struct ENGINE_EXPORT deviceDescription final
    {
        DEFINE_DATA;

      private:
        friend instance;
        deviceDescription() = delete;
        deviceDescription( instance *instance, VkPhysicalDevice phDevice );
        ~deviceDescription();

      public:
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
        VkFormat formatPriority( const std::vector<VkFormat> &formats, VkImageTiling ImageTiling, VkFormatFeatureFlags FormatFeatureFlags ) const noexcept;
        uint32_t requeredMemoryTypeIndex( uint32_t type, VkMemoryPropertyFlags properties ) const;
        types::swapchain getLink( window::types::window window ) const noexcept;
        types::queue getQueue( uint32_t familyIndex, uint32_t index ) const noexcept;
        VkShaderModule loadShader( const char *path, VkShaderModuleCreateFlags flags = 0, const void *pNext = 0 );
        class _memory final
        {
            DEFINE_DATA;

          public:
            _memory() = delete;
            _memory( types::device device );
            ~_memory();
            struct allocationAddres
            {
                VkDeviceSize size;
                VkDeviceSize offset;
                VkDeviceMemory memory;
                uint32_t memoryType;

              private:
                friend device;
                void *block;
            };

            allocationAddres allocate( VkImage image, VkMemoryPropertyFlags flags );
            allocationAddres allocate( VkBuffer buffer, VkMemoryPropertyFlags flags );
            void free( allocationAddres &addres );
        };
        _memory *memory;
        types::queue universalQueue;
        VkDevice handle { nullptr };
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
        struct image_T final
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

    class ENGINE_EXPORT image final
    {
        DEFINE_DATA;

      public:
        image() = delete;
        image( types::device device, VkImageCreateInfo ImageCreateInfo, VkImageViewCreateInfo ImageViewCreateInfo, VkMemoryPropertyFlags memoryPropertiesFlag );
        image( types::device device, types::image parent, VkImageViewCreateInfo ImageViewCreateInfo );
        ~image();
        void write( types::commandBuffer commandBuffer, std::vector<char> data, VkExtent3D srcExtend = { 0, 0, 0 }, VkOffset3D srcOffset = { 0, 0, 0 }, VkImageAspectFlags srcAspectMask = VK_IMAGE_ASPECT_COLOR_BIT, uint32_t dstMipLevel = 0, uint32_t arrayLayersCount = 1, uint32_t dstBaseArrayLayer = 0, VkMemoryMapFlags flags = 0 );
        void write( types::commandPool commandPool, std::vector<char> data, VkExtent3D srcExtend = { 0, 0, 0 }, VkOffset3D srcOffset = { 0, 0, 0 }, VkImageAspectFlags srcAspectMask = VK_IMAGE_ASPECT_COLOR_BIT, uint32_t dstMipLevel = 0, uint32_t arrayLayersCount = 1, uint32_t dstBaseArrayLayer = 0, VkMemoryMapFlags flags = 0 );
        void transition( types::commandBuffer commandBuffer, VkImageLayout newLayout, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkDependencyFlags dependencyFlags, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkImageAspectFlags aspectMask, uint32_t srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED, uint32_t dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED );
        void transition( types::commandPool commandPool, VkImageLayout newLayout, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkDependencyFlags dependencyFlags, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkImageAspectFlags aspectMask, uint32_t srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED, uint32_t dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED );
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

        device::_memory::allocationAddres addres;
        VkImage handle { nullptr };
    };

    class ENGINE_EXPORT buffer final
    {
        DEFINE_DATA;

      public:
        buffer() = delete;
        buffer( types::device device, VkBufferCreateInfo BufferCreateInfo, VkMemoryPropertyFlags memoryPropertiesFlag );
        ~buffer();
        void write( std::vector<char> &data, uint32_t offset = 0, VkMemoryMapFlags flags = 0 );
        VkBuffer handle { nullptr };
    };

    class ENGINE_EXPORT queue final
    {
        DEFINE_DATA;

      public:
        queue() = delete;
        queue( types::device device, uint32_t familyIndex, uint32_t queueIndex, float priority = 1.f );
        ~queue();
        // types::queue operator=( types::queue queue ) noexcept;
        bool operator==( const queue &right ) const noexcept;
        VkQueue handle { nullptr };
        uint32_t familyIndex;
        uint32_t index { 0 };
        float priority { 1.f };
    };

    class ENGINE_EXPORT commandPool final
    {
        DEFINE_DATA;

      public:
        commandPool() = delete;
        commandPool( types::queue queue, VkCommandPoolCreateFlags flags );
        ~commandPool();
        void reset( VkCommandPoolResetFlags flags );
        VkCommandPool handle { nullptr };
    };

    class ENGINE_EXPORT commandBuffer final
    {
        DEFINE_DATA;

      public:
        commandBuffer() = delete;
        commandBuffer( types::commandPool commandPool, VkCommandBufferLevel level );
        ~commandBuffer();
        void begin();
        void end();
        void submit( VkFence fence );
        VkCommandBuffer handle { nullptr };
    };

    class ENGINE_EXPORT renderPass
    {
        DEFINE_DATA;

      public:
        renderPass() = delete;
        renderPass( types::device device, VkRenderPassCreateInfo createInfo );
        renderPass( bool, types::device device, VkRenderPassCreateInfo createInfo );
        ~renderPass();
        bool compatible( types::renderPass right );
        VkRenderPass handle { nullptr };
    };

    class ENGINE_EXPORT framebuffer
    {
        DEFINE_DATA;

      public:
        framebuffer() = delete;
        framebuffer( types::renderPass renderPass, std::vector<types::image> attachments );
        framebuffer( bool, types::renderPass renderPass, std::vector<types::image> attachments );
        ~framebuffer();
        VkFramebuffer handle { nullptr };
    };

    class ENGINE_EXPORT instance
    {
        DEFINE_DATA;

      public:
        instance( const char *appName, uint32_t appVersion );
        instance( bool, const char *appName, uint32_t appVersion );
        ~instance();
        const std::vector<types::deviceDescription> &getDevices();
        VkInstance handle { nullptr };
    };
} // namespace Engine

#endif
