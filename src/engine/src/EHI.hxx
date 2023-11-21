#pragma once
#include <common/globals.hxx>
#include <common/logging.hxx>
#include <platform.hxx>
#include <device.hxx>
#include <image.hxx>
#include <queue.hxx>

#define ENGINE_VERSION VK_MAKE_VERSION( 0, 0, 1 )

namespace Engine
{
    struct instance::DATA_TYPE
    {
        void setupDebugLayerCallback();
        void destroyDebugLayerCallback();
        void setLayers( std::vector<const char *> layers );
        void setExtensions( std::vector<const char *> extrensions );
        virtual void setupExtensions( std::vector<const char *> &rExtensions );
        virtual void setupLayers( std::vector<const char *> &rLayers );
        virtual void setupNextChain( const void *&pNext, std::vector<void *> &dataPointers );
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
        void *userPointer { nullptr };
        ~DATA_TYPE();
    };
} // namespace Engine