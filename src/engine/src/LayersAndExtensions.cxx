#include <ObjectiveVulkan/instance.hxx>
#include <ObjectiveVulkan/device.hxx>
#include <ObjectiveVulkan/common/logging.hxx>

namespace Engine
{
    namespace tools
    {
        std::vector<const char *> DefaultInstanceLayers {
            "VK_LAYER_KHRONOS_validation" };
        std::vector<const char *> DefaultInstanceExtensions {
            VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
            VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
            VK_KHR_SURFACE_EXTENSION_NAME,
        };
        std::vector<const char *> DefaultDeviceExtensions {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    } // namespace tools

    void device::OBJECTIVE_VULKAN_DATA_TYPE::setExtensions( std::vector<const char *> &rExtensions )
    {
        extensions.insert( extensions.end(), rExtensions.begin(), rExtensions.end() );
        extensions.insert( extensions.end(), tools::DefaultDeviceExtensions.begin(), tools::DefaultDeviceExtensions.end() );
    }

    bool device::OBJECTIVE_VULKAN_DATA_TYPE::supportExtensions()
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
                e += s;
            }
            SPDLOG_CRITICAL( e );
        }

        return tmpRequeredDeviceExts.empty();
    }

    void instance::OBJECTIVE_VULKAN_DATA_TYPE::setLayers( std::vector<const char *> nLayers )
    {
        layers.insert( layers.end(), nLayers.begin(), nLayers.end() );
        layers.insert( layers.end(), tools::DefaultInstanceLayers.begin(), tools::DefaultInstanceLayers.end() );
    }

    void instance::OBJECTIVE_VULKAN_DATA_TYPE::setExtensions( std::vector<const char *> nExtensions )
    {
        extensions.insert( extensions.end(), nExtensions.begin(), nExtensions.end() );
        extensions.insert( extensions.end(), tools::DefaultInstanceExtensions.begin(), tools::DefaultInstanceExtensions.end() );
    }

    bool instance::OBJECTIVE_VULKAN_DATA_TYPE::supportLayers()
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

    bool instance::OBJECTIVE_VULKAN_DATA_TYPE::supportExtensions()
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