#pragma once
#ifndef EHI_HXX
#    define EHI_HXX

#    include <common/globals.hxx>
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
        VkDebugUtilsMessengerCreateInfoEXT setupDebugLayerCallback();
        void initDebugLayerCallBack( VkDebugUtilsMessengerCreateInfoEXT createInfo );
        void destroyDebugLayerCallback();
        bool supportLayers();
        bool supportExtensions();
        void create( VkInstanceCreateInfo createInfo );
        std::vector<const char *> layers;
        std::vector<const char *> extensions;
        VkDebugUtilsMessengerEXT debugMessenger { nullptr };
        instance *parent;
        std::unordered_map<types::surface, std::vector<types::swapchain>> surfaces;
        std::vector<types::deviceDescription> deviceDescriptions;
        std::set<types::device> devices;
        // std::vector<types::pass> passes;
        ~DATA_TYPE() {}

      private:
        friend instance;
        void setLayers( std::vector<const char *> layers );
        void setExtensions( std::vector<const char *> extrensions );
    };
} // namespace Engine
#endif