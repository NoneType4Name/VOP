#include <RHI.hxx>

namespace Engine
{
    namespace
    {
        namespace tools
        {
            namespace
            {
                VkInstance _instance{ nullptr };
                std::vector<VkPhysicalDevice> _aviliable_phDevices{};
                std::vector<VkPhysicalDeviceProperties> _aviliable_phDevicesProperties{};
            } // namespace

            void createInstance()
            {
                VkApplicationInfo ApplicationInfo{};
                ApplicationInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
                ApplicationInfo.engineVersion      = 0;
                ApplicationInfo.apiVersion         = VK_API_VERSION_1_0;
                ApplicationInfo.pApplicationName   = nullptr;
                ApplicationInfo.applicationVersion = ENGINE_VERSION;

                VkInstanceCreateInfo InstanceCreateInfo{};
                InstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

                VkValidationFeatureEnableEXT enabled[]{ VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT };
                VkValidationFeaturesEXT ValidationFeatures{};
                ValidationFeatures.sType                         = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
                ValidationFeatures.enabledValidationFeatureCount = sizeof( enabled ) / sizeof( enabled[ 0 ] );
                ValidationFeatures.pEnabledValidationFeatures    = enabled;
#ifdef ENGINE_DEBUG
                InstanceCreateInfo.pNext = &ValidationFeatures;
#endif

                std::vector<const char *> Extensions{}, Layers{};
                Engine::tools::getInstanceLayersAndExtension( Extensions, Layers );
                InstanceCreateInfo.enabledLayerCount       = Layers.size();
                InstanceCreateInfo.ppEnabledLayerNames     = Layers.size() ? Layers.data() : nullptr;
                InstanceCreateInfo.enabledExtensionCount   = Extensions.size();
                InstanceCreateInfo.ppEnabledExtensionNames = Extensions.size() ? Extensions.data() : nullptr;
                InstanceCreateInfo.pApplicationInfo        = &ApplicationInfo;
                CHECK_RESULT( vkCreateInstance( &InstanceCreateInfo, nullptr, &_instance ) );
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
    }     // namespace
} // namespace Engine