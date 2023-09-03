#include <RHI.hxx>

namespace Engine
{
    namespace tools
    {
        namespace
        {
            VkInstance _instance{ nullptr };
        }
        void createInstance()
        {
            //             VkInstanceCreateInfo InstanceCreateInfo{};
            //             InstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            //             uint32_t glfwExtensionsCount{ 0 };
            //             const char **glfwExtensions = glfwGetRequiredInstanceExtensions( &glfwExtensionsCount );
            //             std::vector<const char *> glfwExtensionsVector( glfwExtensions, glfwExtensions + glfwExtensionsCount );

            // #ifdef _DEBUG
            //             uint32_t _c;
            //             vkEnumerateInstanceLayerProperties( &_c, nullptr );
            //             std::vector<VkLayerProperties> AviableLayers{ _c };
            //             vkEnumerateInstanceLayerProperties( &_c, AviableLayers.data() );
            //             size_t c{ ValidationLayers.size() };
            //             std::vector<const char *> NotAvilableLayers{ ValidationLayers.begin(), ValidationLayers.end() };
            //             for( size_t i{ 0 }; i < c; i++ )
            //             {
            //                 for( uint32_t _i{ 0 }; _i < _c; _i++ )
            //                     if( !strcmp( NotAvilableLayers[ c - 1 - i ], AviableLayers[ _i ].layerName ) )
            //                     {
            //                         NotAvilableLayers.erase( NotAvilableLayers.end() - i - 1 );
            //                         break;
            //                     }
            //                 if( NotAvilableLayers.empty() )
            //                     break;
            //             }
            //             if( !NotAvilableLayers.empty() )
            //             {
            //                 std::string Err = std::format( "Not finded validation layers: {}:\n", std::to_string( NotAvilableLayers.size() ) );
            //                 for( const auto VL : NotAvilableLayers )
            //                 {
            //                     Err += std::format( "\t {}\n", VL );
            //                 }
            //                 SPDLOG_CRITICAL( Err );
            //             }

            //             VkValidationFeatureEnableEXT enabled[]{ VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT };
            //             VkValidationFeaturesEXT ValidationFeatures{};
            //             ValidationFeatures.sType                          = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
            //             ValidationFeatures.enabledValidationFeatureCount  = sizeof( enabled ) / sizeof( enabled[ 0 ] );
            //             ValidationFeatures.pEnabledValidationFeatures     = enabled;
            //             ValidationFeatures.disabledValidationFeatureCount = 0;
            //             ValidationFeatures.pDisabledValidationFeatures    = nullptr;

            //             VkDebugUtilsMessengerCreateInfoEXT DebugUtilsMessengerCreateInfoEXT{};
            //             InstanceCreateInfo.pNext                         = &DebugUtilsMessengerCreateInfoEXT;
            //             DebugUtilsMessengerCreateInfoEXT.sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            //             DebugUtilsMessengerCreateInfoEXT.pNext           = &ValidationFeatures;
            //             DebugUtilsMessengerCreateInfoEXT.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
            //             DebugUtilsMessengerCreateInfoEXT.messageType     = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            //             DebugUtilsMessengerCreateInfoEXT.pfnUserCallback = DebugCallback;
            //             // DebugUtilsMessengerCreateInfoEXT.pUserData       = this; // No used.
            //             glfwExtensionsVector.push_back( VK_EXT_DEBUG_UTILS_EXTENSION_NAME );
            //             InstanceCreateInfo.enabledLayerCount   = ValidationLayers.size();
            //             InstanceCreateInfo.ppEnabledLayerNames = ValidationLayers.data();

            // #endif
            //             InstanceCreateInfo.enabledExtensionCount   = glfwExtensionsVector.size();
            //             InstanceCreateInfo.ppEnabledExtensionNames = glfwExtensionsVector.data();
            //             VkApplicationInfo ApplicationInfo{};
            //             InstanceCreateInfo.pApplicationInfo = &ApplicationInfo;
            //             ApplicationInfo.sType               = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            //             ApplicationInfo.engineVersion       = 0;
            //             ApplicationInfo.apiVersion          = VK_API_VERSION_1_0;
            //             auto app_name{ "Grapchic engine of " + _title };
            //             ApplicationInfo.pApplicationName   = app_name.c_str();
            //             ApplicationInfo.applicationVersion = ENGINE_VERSION;
            //             VkResult Result{ vkCreateInstance( &InstanceCreateInfo, nullptr, &_instance ) };
        }

        VkInstance getInstance()
        {
            return _instance;
        }
    } // namespace tools
} // namespace Engine