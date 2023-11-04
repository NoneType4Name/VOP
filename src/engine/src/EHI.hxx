#pragma once
#include <common/globals.hxx>
#include <common/logging.hxx>
#include <platform.hxx>
#include <device.hxx>
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
        std::string title {};
        ResizeCallback resizeCallBack { nullptr };
        KeyEventCallBack eventCallBack { nullptr };
        GLFWwindow *window { nullptr };
        VkSurfaceKHR surface { nullptr };
        VkInstance instance { nullptr };
    };

    struct instance::data
    {
      public:
        void setupDeviceDescriptions();
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
        VkDebugUtilsMessengerEXT debugMessenger;
        VkInstance handle;
        ~data();
    };

    struct DeviceDescription::data
    {
      public:
        data() = default;
        void init( VkPhysicalDevice device );
        std::vector<VkQueueFamilyProperties> queueFamilyProperties;
        VkPhysicalDeviceMemoryProperties memProperties {};
        VkPhysicalDeviceProperties properties {};
        VkPhysicalDeviceFeatures features {};
        VkPhysicalDevice phDevice { nullptr };
        ~data() = default;
    };

    struct device::data
    {
      public:
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
        window::window *window;
        DeviceDescription *description;
        tools::queueSet queuesSet;
    };

    class link::data
    {
        virtual void setup##();
        window::types::window window;
        types::device device;
    };
    namespace tools
    {
    } // namespace tools
} // namespace Engine