#include <platform.hxx>
#include <EHI.hxx>

namespace Engine
{
    namespace tools
    {
        std::vector<VkValidationFeatureEnableEXT> DefaultValidationFeatures {
            VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT };
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
            VK_KHR_MAINTENANCE3_EXTENSION_NAME,
            VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME
#endif
        };

    } // namespace tools

    void Instance::setupExtensions( std::vector<const char *> &rExtensions ) {}
    void Instance::setupLayers( std::vector<const char *> &rLayers ) {}

    void getDeviceExtensions( std::vector<std::string> &deviceExtensions )
    {
        deviceExtensions.insert( deviceExtensions.end(), tools::DefaultDeviceExtensions.begin(), tools::DefaultDeviceExtensions.end() );
    }

    void Instance::data::setLayers( std::vector<const char *> nLayers )
    {
        layers.insert( layers.end(), nLayers.begin(), nLayers.end() );
        // #ifdef ENGINE_DEBUG
        layers.insert( layers.end(), tools::DefaultInstanceLayers.begin(), tools::DefaultInstanceLayers.end() );
        // #endif
    }

    void Instance::data::setValidationFeatures( std::vector<VkValidationFeatureEnableEXT> &rVFeatures )
    {
        validationFeatures.insert( validationFeatures.end(), rVFeatures.begin(), rVFeatures.end() );
        validationFeatures.insert( validationFeatures.end(), tools::DefaultValidationFeatures.begin(), tools::DefaultValidationFeatures.end() );
        uint32_t _c;
        const char **req_exts { glfwGetRequiredInstanceExtensions( &_c ) };
        extensions.insert( extensions.end(), &req_exts[ 0 ], &req_exts[ _c ] );
    }

    void Instance::data::setExtensions( std::vector<const char *> nExtensions )
    {
        extensions.insert( extensions.end(), nExtensions.begin(), nExtensions.end() );
        extensions.insert( extensions.end(), tools::DefaultInstanceExtensions.begin(), tools::DefaultInstanceExtensions.end() );
        uint32_t _c;
        const char **req_exts { glfwGetRequiredInstanceExtensions( &_c ) };
        extensions.insert( extensions.end(), &req_exts[ 0 ], &req_exts[ _c ] );
    }

    bool Instance::data::supportLayers()
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

    bool Instance::data::supportExtensions()
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
} // namespace Engine