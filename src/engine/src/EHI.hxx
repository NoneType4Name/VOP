#pragma once
#include <common/globals.hxx>
#include <common/logging.hxx>
#include <platform.hxx>
#include <device.hxx>
#include <image.hxx>
#include <engine.hxx>

#define ENGINE_VERSION VK_MAKE_VERSION( 0, 0, 1 )

namespace Engine
{
    struct window::window::data
    {
        virtual void setupNextChain( const void *&pNext );
        virtual void setupFlags( VkWin32SurfaceCreateFlagsKHR flags );
        void createSurface( VkInstance instance, const void *pNext, VkWin32SurfaceCreateFlagsKHR flags );
        void destroySurface();
        RESOLUTION_TYPE width { 0 };
        RESOLUTION_TYPE height { 0 };
        std::string title;
        ResizeCallback resizeCallBack { nullptr };
        KeyEventCallBack eventCallBack { nullptr };
        GLFWwindow *window { nullptr };
        VkSurfaceKHR surface { nullptr };
        VkInstance instance { nullptr };
    };

    struct instance::data
    {
        void setupDebugLayerCallback();
        void destroyDebugLayerCallback();
        void setLayers( std::vector<const char *> layers );
        void setExtensions( std::vector<const char *> extrensions );
        virtual void setupExtensions( std::vector<const char *> &rExtensions );
        virtual void setupLayers( std::vector<const char *> &rLayers );
        virtual void setupNextChain( const void *&pNext );
        bool supportLayers();
        bool supportExtensions();
        std::vector<std::unique_ptr<window::window>> windows;
        std::vector<std::unique_ptr<DeviceDescription>> deviceDescriptions;
        std::vector<std::unique_ptr<device>> devices;
        std::vector<std::unique_ptr<link>> links;
        std::vector<std::string> layers;
        std::vector<std::string> extensions;
        VkDebugUtilsMessengerEXT debugMessenger { nullptr };
        VkInstance handle { nullptr };
        void *pUserData { nullptr };
        ~data();
    };

    struct DeviceDescription::data
    {
        data() = default;
        void init( VkPhysicalDevice device );
        std::vector<VkQueueFamilyProperties> queueFamilyProperties;
        VkPhysicalDeviceMemoryProperties memProperties {};
        VkPhysicalDeviceProperties properties {};
        VkPhysicalDeviceFeatures features {};
        VkPhysicalDevice phDevice { nullptr };
    };

    struct device::data
    {
        virtual void setupNextChain( const void *&nextChainData );
        virtual void setupExtensions( std::vector<const char *> &deviceExtensions );
        virtual void setupQueueSet( tools::queueSet &queues, std::vector<VkQueueFamilyProperties> avilavleQueues );
        virtual void setupFeatures( VkPhysicalDeviceFeatures &features );
        virtual void setupQueues( tools::queueSet &queues );
        void setExtensions( std::vector<const char *> &deviceExtensions );
        bool supportExtensions();
        void init();
        VkDevice device { nullptr };
        std::vector<std::string> extensions;
        window::window *window { nullptr };
        DeviceDescription *description { nullptr };
        tools::queueSet queuesSet;
    };

    struct link::data
    {
        struct properties_T
        {
            properties_T( window::types::window window, types::device device );
            VkSurfaceCapabilitiesKHR capabilities;
            std::vector<VkSurfaceFormatKHR> formats;
            std::vector<VkPresentModeKHR> presentModes;
        };

        struct image_T
        {
            image *image { nullptr };
            VkSemaphore isAvailable { nullptr };
            VkSemaphore isRendered { nullptr };
        };

        virtual void setupCreateInfo( VkSwapchainCreateInfoKHR &createInfo );
        void init( window::types::window window, types::device device );
        void setup();

        window::types::window window { nullptr };
        types::device device { nullptr };
        VkSwapchainKHR swapchain { nullptr };
        VkSurfaceFormatKHR format { VK_FORMAT_MAX_ENUM };
        VkPresentModeKHR presentMode { VK_PRESENT_MODE_MAX_ENUM_KHR };
        VkFormat depthImageFormat { VK_FORMAT_MAX_ENUM };
        properties_T properties;
        std::vector<image_T> images;
        uint32_t flightImgIndex { 0 };
        uint32_t semaphoreIndex { 0 };
        VkSwapchainCreateInfoKHR createInfo {};
    };
    namespace tools
    {
    } // namespace tools
} // namespace Engine