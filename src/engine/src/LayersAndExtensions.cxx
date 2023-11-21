#include <platform.hxx>
#include <EHI.hxx>
#include <device.hxx>

namespace Engine
{
    namespace tools
    {
        std::vector<const char *> DefaultInstanceLayers {
#ifdef PLATFORM_WINDOWS
            "VK_LAYER_KHRONOS_validation"
#endif
        };
        std::vector<const char *> DefaultInstanceExtensions {
            VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
            VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
            VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME
#ifdef PLATFORM_WINDOWS
#endif
        };
        std::vector<const char *> DefaultDeviceExtensions {
#ifdef PLATFORM_WINDOWS
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            VK_KHR_MAINTENANCE3_EXTENSION_NAME
#endif
        };

    } // namespace tools

    void device::DATA_TYPE::setExtensions( std::vector<const char *> &rExtensions )
    {
        extensions.insert( extensions.end(), rExtensions.begin(), rExtensions.end() );
        extensions.insert( extensions.end(), tools::DefaultDeviceExtensions.begin(), tools::DefaultDeviceExtensions.end() );
    }

    bool device::DATA_TYPE::supportExtensions()
    {
        uint32_t _c { 0 };
        vkEnumerateDeviceExtensionProperties( description->data->phDevice, nullptr, &_c, nullptr );
        std::vector<VkExtensionProperties> AvailableExtNames { _c };
        vkEnumerateDeviceExtensionProperties( description->data->phDevice, nullptr, &_c, AvailableExtNames.data() );
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

    void instance::DATA_TYPE::setLayers( std::vector<const char *> nLayers )
    {
        layers.insert( layers.end(), nLayers.begin(), nLayers.end() );
        // #ifdef ENGINE_DEBUG
        layers.insert( layers.end(), tools::DefaultInstanceLayers.begin(), tools::DefaultInstanceLayers.end() );
        // #endif
    }

    void instance::DATA_TYPE::setExtensions( std::vector<const char *> nExtensions )
    {
        extensions.insert( extensions.end(), nExtensions.begin(), nExtensions.end() );
        extensions.insert( extensions.end(), tools::DefaultInstanceExtensions.begin(), tools::DefaultInstanceExtensions.end() );
        uint32_t _c;
        const char **req_exts { glfwGetRequiredInstanceExtensions( &_c ) };
        extensions.insert( extensions.end(), &req_exts[ 0 ], &req_exts[ _c ] );
    }

    bool instance::DATA_TYPE::supportLayers()
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
            std::string e { "Not avilable instance layers:\n" };
            for ( const auto &ext : tmpRequeredDeviceL )
            {
                auto s { std::format( "\t{}\n", ext ) };
                e += s;
            }
            SPDLOG_CRITICAL( e );
        }
        return tmpRequeredDeviceL.empty();
    }

    bool instance::DATA_TYPE::supportExtensions()
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
            std::string e { "Not avilable instance extensions:\n" };
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