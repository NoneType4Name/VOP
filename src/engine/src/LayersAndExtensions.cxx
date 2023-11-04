#include <platform.hxx>
#include <EHI.hxx>
#include <device.hxx>

namespace Engine
{
    namespace tools
    {
        std::vector<std::string> DefaultInstanceLayers {
#ifdef PLATFORM_WINDOWS
            "VK_LAYER_KHRONOS_validation"
#endif
        };
        std::vector<std::string> DefaultInstanceExtensions {
            VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
            VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME
#ifdef PLATFORM_WINDOWS
#endif
        };
        std::vector<std::string> DefaultDeviceExtensions {
#ifdef PLATFORM_WINDOWS
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            VK_KHR_MAINTENANCE3_EXTENSION_NAME
#endif
        };

    } // namespace tools

    void instance::data::setupExtensions( std::vector<const char *> &rExtensions ) {}
    void instance::data::setupLayers( std::vector<const char *> &rLayers ) {}

    void device::data::setExtensions( std::vector<const char *> &rExtensions )
    {
        extensions.insert( extensions.end(), rExtensions.begin(), rExtensions.end() );
        extensions.insert( extensions.end(), tools::DefaultDeviceExtensions.begin(), tools::DefaultDeviceExtensions.end() );
    }

    void instance::data::setLayers( std::vector<const char *> nLayers )
    {
        layers.insert( layers.end(), nLayers.begin(), nLayers.end() );
        // #ifdef ENGINE_DEBUG
        layers.insert( layers.end(), tools::DefaultInstanceLayers.begin(), tools::DefaultInstanceLayers.end() );
        // #endif
    }

    void instance::data::setupNextChain( const void *&pNext )
    {
        VkValidationFeatureEnableEXT enabled[] { VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT };
        VkValidationFeaturesEXT ValidationFeatures {};
        pNext                                             = &ValidationFeatures;
        ValidationFeatures.sType                          = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
        ValidationFeatures.enabledValidationFeatureCount  = sizeof( enabled ) / sizeof( enabled[ 0 ] );
        ValidationFeatures.pEnabledValidationFeatures     = enabled;
        ValidationFeatures.disabledValidationFeatureCount = 0;
        ValidationFeatures.pDisabledValidationFeatures    = nullptr;
    }

    void instance::data::setExtensions( std::vector<const char *> nExtensions )
    {
        extensions.insert( extensions.end(), nExtensions.begin(), nExtensions.end() );
        extensions.insert( extensions.end(), tools::DefaultInstanceExtensions.begin(), tools::DefaultInstanceExtensions.end() );
        uint32_t _c;
        const char **req_exts { glfwGetRequiredInstanceExtensions( &_c ) };
        extensions.insert( extensions.end(), &req_exts[ 0 ], &req_exts[ _c ] );
    }

    bool instance::data::supportLayers()
    {
        uint32_t _c { 0 };
        vkEnumerateInstanceLayerProperties( &_c, nullptr );
        std::vector<VkLayerProperties> AvailableLNames { _c };
        vkEnumerateInstanceLayerProperties( &_c, AvailableLNames.data() );
        std::set<std::string> tmpRequeredDeviceL { layers.begin(), layers.end() };
        for ( const auto &l : AvailableLNames )
        {
            tmpRequeredDeviceL.erase( l.layerName );
        }
        if ( tmpRequeredDeviceL.size() )
        {
            std::string e { "Not avilable instance extensions:\n" };
            for ( const auto &ext : tmpRequeredDeviceL )
            {
                auto s { std::format( "\t{}\n", ext ) };
                e += s;
            }
            SPDLOG_CRITICAL( e );
        }
        return tmpRequeredDeviceL.empty();
    }

    bool instance::data::supportExtensions()
    {
        uint32_t _c { 0 };
        vkEnumerateInstanceExtensionProperties( nullptr, &_c, nullptr );
        std::vector<VkExtensionProperties> AvailableExtNames { _c };
        vkEnumerateInstanceExtensionProperties( nullptr, &_c, AvailableExtNames.data() );
        std::set<std::string> tmpRequeredDeviceExts { extensions.begin(), extensions.end() };
        for ( const auto &ext : AvailableExtNames )
        {
            tmpRequeredDeviceExts.erase( ext.extensionName );
        }

        if ( tmpRequeredDeviceExts.size() )
        {
            std::string e { "Not avilable device extensions:\n" };
            for ( const auto &ext : tmpRequeredDeviceExts )
            {
                auto s { std::format( "\t{}\n", ext ) };
                e += s;
            }
            SPDLOG_CRITICAL( e );
        }

        return tmpRequeredDeviceExts.empty();
    }

    bool device::data::supportExtensions()
    {
        uint32_t _c { 0 };
        vkEnumerateDeviceExtensionProperties( phDevice, nullptr, &_c, nullptr );
        std::vector<VkExtensionProperties> AvailableExtNames { _c };
        vkEnumerateDeviceExtensionProperties( phDevice, nullptr, &_c, AvailableExtNames.data() );
        std::set<std::string> tmpRequeredDeviceExts { extensions.begin(), extensions.end() };
        for ( const auto &ext : AvailableExtNames )
        {
            tmpRequeredDeviceExts.erase( ext.extensionName );
        }

        if ( tmpRequeredDeviceExts.size() )
        {
            std::string e { "Not avilable device extensions:\n" };
            for ( const auto &ext : tmpRequeredDeviceExts )
            {
                auto s { std::format( "\t{}\n", ext ) };
            }
            SPDLOG_CRITICAL( e );
        }

        return tmpRequeredDeviceExts.empty();
    }
} // namespace Engine