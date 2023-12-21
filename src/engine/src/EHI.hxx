#pragma once
#ifndef EHI_HXX
#    define EHI_HXX
#    include <platform.hxx>
#    include <common/globals.hxx>
#    include <common/logging.hxx>
#    include <engine.hxx>
// #    include <device.hxx>
// #    include <image.hxx>
// #    include <queue.hxx>
// #    include <engine_setup.hxx>

#    define ENGINE_VERSION VK_MAKE_VERSION( 0, 0, 1 )

namespace Engine
{
    struct instance::DATA_TYPE
    {
        DATA_TYPE( instance *parent ) :
            parent { parent } {}
        void setupDebugLayerCallback( VkDebugUtilsMessengerCreateInfoEXT &createInfo );
        void initDebugLayerCallBack( VkDebugUtilsMessengerCreateInfoEXT createInfo );
        void destroyDebugLayerCallback();
        bool supportLayers();
        bool supportExtensions();
        void init( VkInstanceCreateInfo createInfo );
        std::vector<const char *> layers;
        std::vector<const char *> extensions;
        VkDebugUtilsMessengerEXT debugMessenger { nullptr };
        VkInstance handle { nullptr };
        instance *parent { nullptr };
        ~DATA_TYPE();
        friend instance;

      private:
        void setLayers( std::vector<const char *> layers );
        void setExtensions( std::vector<const char *> extrensions );
        // std::vector<std::unique_ptr<window::window>> windows;
        // std::vector<std::unique_ptr<DeviceDescription>> deviceDescriptions;
        // std::vector<std::unique_ptr<device>> devices;
        // std::vector<std::unique_ptr<link>> links;
        // std::vector<std::unique_ptr<pass>> passes;
    };
} // namespace Engine
#endif