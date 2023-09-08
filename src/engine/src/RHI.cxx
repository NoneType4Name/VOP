#include <RHI.hxx>

namespace Engine
{
    namespace tools
    {
        namespace
        {
            VkInstance _instance{ nullptr };
            std::vector<VkPhysicalDevice> _aviliable_phDevices{};
            std::vector<VkPhysicalDeviceProperties> _aviliable_phDevicesProperties{};
        } // namespace

        bool instanseSupportExtensionsAndLayers( std::vector<const char *> extensions, std::vector<const char *> layers )
        {
            uint32_t _c{ 0 };
            vkEnumerateInstanceExtensionProperties( nullptr, &_c, nullptr );
            std::vector<VkExtensionProperties> AvilableExtNames{ _c };
            vkEnumerateInstanceExtensionProperties( nullptr, &_c, AvilableExtNames.data() );
            std::set<std::string> tmpRequeredDeviceExts{ extensions.begin(), extensions.end() };
            for( const auto &ext : AvilableExtNames )
            {
                tmpRequeredDeviceExts.erase( ext.extensionName );
            }

            vkEnumerateInstanceLayerProperties( &_c, nullptr );
            std::vector<VkLayerProperties> AvilableLNames{ _c };
            vkEnumerateInstanceLayerProperties( &_c, AvilableLNames.data() );
            std::set<std::string> tmpRequeredDeviceL{ layers.begin(), layers.end() };
            for( const auto &l : AvilableLNames )
            {
                tmpRequeredDeviceL.erase( l.layerName );
            }

            if( tmpRequeredDeviceL.size() )
            {
                std::string e{ "Not avilable instance extensions:\n" };
                for( const auto &ext : tmpRequeredDeviceL )
                {
                    auto s{ std::format( "\t{}\n", ext ) };
                    e += s;
                }
                SPDLOG_CRITICAL( e );
            }
            if( tmpRequeredDeviceL.size() )
            {
                std::string e{ "Not avilable device extensions:\n" };
                for( const auto &l : tmpRequeredDeviceL )
                {
                    auto s{ std::format( "\t{}\n", l ) };
                    e += s;
                }
                SPDLOG_CRITICAL( e );
            }

            return tmpRequeredDeviceExts.empty() && tmpRequeredDeviceL.empty();
        }

        void createInstance()
        {
            VkApplicationInfo ApplicationInfo{};
            ApplicationInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            ApplicationInfo.engineVersion      = 0;
            ApplicationInfo.apiVersion         = VK_API_VERSION_1_0;
            ApplicationInfo.pApplicationName   = nullptr;
            ApplicationInfo.applicationVersion = ENGINE_VERSION;

            VkValidationFeatureEnableEXT enabled[]{ VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT };
            VkValidationFeaturesEXT ValidationFeatures{};
            ValidationFeatures.sType                         = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
            ValidationFeatures.enabledValidationFeatureCount = sizeof( enabled ) / sizeof( enabled[ 0 ] );
            ValidationFeatures.pEnabledValidationFeatures    = enabled;

            VkInstanceCreateInfo InstanceCreateInfo{};
            InstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
#ifdef ENGINE_DEBUG
            InstanceCreateInfo.pNext = &ValidationFeatures;
#endif
            std::vector<const char *> Extensions{}, Layers{};
            getInstanceLayersAndExtension( Extensions, Layers );
            assert( instanseSupportExtensionsAndLayers( Extensions, Layers ) );
            InstanceCreateInfo.enabledLayerCount       = Layers.size();
            InstanceCreateInfo.ppEnabledLayerNames     = Layers.size() ? Layers.data() : nullptr;
            InstanceCreateInfo.enabledExtensionCount   = Extensions.size();
            InstanceCreateInfo.ppEnabledExtensionNames = Extensions.size() ? Extensions.data() : nullptr;
            InstanceCreateInfo.pApplicationInfo        = &ApplicationInfo;
            CHECK_RESULT( vkCreateInstance( &InstanceCreateInfo, nullptr, &_instance ) );
        }

        void destroyInstance()
        {
            vkDestroyInstance( _instance, ALLOCATION_CALLBACK );
        }

        VkQueue estroyInstance()
        {
            return nullptr;
        }

        std::vector<VkPhysicalDevice> getAviliableDevices()
        {
            if( _aviliable_phDevices.empty() )
            {
                uint32_t _c{ 0 };
                CHECK_RESULT( vkEnumeratePhysicalDevices( _instance, &_c, nullptr ) );
                _aviliable_phDevices.reserve( _c );
                _aviliable_phDevices.clear();
                CHECK_RESULT( vkEnumeratePhysicalDevices( _instance, &_c, _aviliable_phDevices.data() ) );
            }
            return _aviliable_phDevices;
        }

        std::vector<VkPhysicalDeviceProperties> getAviliableDevicesProperties()
        {
            if( _aviliable_phDevicesProperties.empty() )
            {
                _aviliable_phDevicesProperties.resize( _aviliable_phDevices.size() );
                for( size_t i{ 0 }; _aviliable_phDevices.size(); i++ )
                    vkGetPhysicalDeviceProperties( _aviliable_phDevices[ i ], &_aviliable_phDevicesProperties[ i ] );
            }

            return _aviliable_phDevicesProperties;
        }

        VkInstance getInstance()
        {
            return _instance;
        }
    } // namespace tools
} // namespace Engine
