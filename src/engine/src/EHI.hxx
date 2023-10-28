#pragma once
#include <common/globals.hxx>
#include <common/logging.hxx>
#include <platform.hxx>
#include <engine.hxx>

#define ENGINE_VERSION VK_MAKE_VERSION( 0, 0, 1 )

namespace Engine
{
    struct Instance::data
    {
      public:
        void setupDebugLayerCallback();
        void destroyDebugLayerCallback();
        void setLayers( std::vector<const char *> layers );
        void setExtensions( std::vector<const char *> extrensions );
        void setValidationFeatures( std::vector<VkValidationFeatureEnableEXT> &rVFeatures );
        bool supportLayers();
        bool supportExtensions();
        std::vector<VkValidationFeatureEnableEXT> validationFeatures;
        std::vector<std::string> layers;
        std::vector<std::string> extensions;
        VkInstance handle;

      private:
        VkDebugUtilsMessengerEXT debugMessenger;
    };

    struct DeviceDescription::data
    {
      public:
        data()
        {
        }
        VkPhysicalDeviceMemoryProperties memProperties;
        VkPhysicalDeviceProperties properties;
        
    };
    namespace tools
    {
        std::vector<DeviceDescription> &setupDeviceDescriptions();
    }
} // namespace Engine