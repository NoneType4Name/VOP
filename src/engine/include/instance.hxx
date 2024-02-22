#pragma once
#ifndef INSTANCE_HXX
#    define INSTANCE_HXX
#    include <common/globals.hxx>

namespace Engine
{
    struct instance::OBJECTIVE_VULKAN_DATA_TYPE
    {
        OBJECTIVE_VULKAN_DATA_TYPE( instance *parent ) :
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
        ~OBJECTIVE_VULKAN_DATA_TYPE() {}

      private:
        friend instance;
        void setLayers( std::vector<const char *> layers );
        void setExtensions( std::vector<const char *> extrensions );
    };
} // namespace Engine
#endif