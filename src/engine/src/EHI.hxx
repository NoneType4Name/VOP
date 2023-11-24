#pragma once
#include <platform.hxx>
#include <device.hxx>
#include <image.hxx>
#include <queue.hxx>
#include <setup_engine.hxx>

#define ENGINE_VERSION VK_MAKE_VERSION( 0, 0, 1 )

namespace Engine
{
    struct instance::DATA_TYPE
    {
        void setupDebugLayerCallback( VkDebugUtilsMessengerCreateInfoEXT &createInfo, std::vector<void *> &pData );
        void initDebugLayerCallBack( VkDebugUtilsMessengerCreateInfoEXT createInfo );
        void destroyDebugLayerCallback();
        void setLayers( std::vector<const char *> layers );
        void setExtensions( std::vector<const char *> extrensions );
        bool supportLayers();
        bool supportExtensions();
        std::vector<std::unique_ptr<window::window>> windows;
        std::vector<std::unique_ptr<DeviceDescription>> deviceDescriptions;
        std::vector<std::unique_ptr<device>> devices;
        std::vector<std::unique_ptr<link>> links;
        std::vector<std::unique_ptr<pass>> passes;
        std::vector<const char *> layers;
        std::vector<const char *> extensions;
        VkDebugUtilsMessengerEXT debugMessenger { nullptr };
        VkInstance handle { nullptr };
        std::unique_ptr<InstanceSetup> setup { nullptr };
        void *userPointer { nullptr };
        ~DATA_TYPE();
    };
} // namespace Engine