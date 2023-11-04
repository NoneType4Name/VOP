#include <EHI.hxx>

namespace Engine
{
    namespace
    {
        VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT MessageLevel,
            VkDebugUtilsMessageTypeFlagsEXT MessageType,
            const VkDebugUtilsMessengerCallbackDataEXT *CallbackData,
            void *UserData )
        {
            std::string StrMessageType;
            switch ( MessageType )
            {
                case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
                    StrMessageType = "GeneralError";
                    break;
                case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
                    StrMessageType = "SpecificationError";
                    break;
                case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
                    StrMessageType = "MisuseVulkanApiError";
                    break;

                default:
                    StrMessageType = string_VkDebugUtilsMessageTypeFlagsEXT( MessageType );
                    break;
            }
            switch ( static_cast<uint32_t>( MessageLevel ) )
            {
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
                    SPDLOG_DEBUG( std::format( "{} message: {}", ( MessageType == VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT ? "" : format( "Type: {}, ", StrMessageType ) ), CallbackData->pMessage ) );
                    break;
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                    SPDLOG_INFO( std::format( "{} message: {}", ( MessageType == VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT ? "" : format( "Type: {}, ", StrMessageType ) ), CallbackData->pMessage ) );
                    break;
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                    SPDLOG_WARN( std::format( "{} message: {}", ( MessageType == VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT ? "" : format( "Type: {}, ", StrMessageType ) ), CallbackData->pMessage ) );
                    break;
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                    SPDLOG_CRITICAL( std::format( "{} message: {}", ( MessageType == VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT ? "" : format( "Type: {}, ", StrMessageType ) ), CallbackData->pMessage ) );
                    return VK_TRUE;
                    break;
            }
            return VK_FALSE;
        }
    } // namespace

    void instance::data::setupDebugLayerCallback()
    {
        // VkValidationFeatureEnableEXT enabled[]{ VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT };
        // VkValidationFeaturesEXT ValidationFeatures{};
        // ValidationFeatures.sType                         = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
        // ValidationFeatures.enabledValidationFeatureCount = sizeof( enabled ) / sizeof( enabled[ 0 ] );
        // ValidationFeatures.pEnabledValidationFeatures    = enabled;

        VkDebugUtilsMessengerCreateInfoEXT DebugUtilsMessengerCreateInfoEXT {};
        DebugUtilsMessengerCreateInfoEXT.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        // DebugUtilsMessengerCreateInfoEXT.pNext           = &ValidationFeatures;
        DebugUtilsMessengerCreateInfoEXT.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        DebugUtilsMessengerCreateInfoEXT.messageType     = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        DebugUtilsMessengerCreateInfoEXT.pfnUserCallback = DebugCallback;
        DebugUtilsMessengerCreateInfoEXT.pUserData       = nullptr;

        auto _crtDbgUtMsgrEXT = ( PFN_vkCreateDebugUtilsMessengerEXT ) vkGetInstanceProcAddr( handle, "vkCreateDebugUtilsMessengerEXT" );
        if ( _crtDbgUtMsgrEXT != nullptr )
        {
            CHECK_RESULT( _crtDbgUtMsgrEXT( handle, &DebugUtilsMessengerCreateInfoEXT, nullptr, &debugMessenger ) );
        }
        else
        {
            SPDLOG_CRITICAL( "Failed to get Debug messenger addres." );
        }
    }

    void instance::data::destroyDebugLayerCallback()
    {
        if ( debugMessenger != VK_NULL_HANDLE )
        {
            PFN_vkDestroyDebugUtilsMessengerEXT destroyMsgCallback = ( PFN_vkDestroyDebugUtilsMessengerEXT ) ( void * ) vkGetInstanceProcAddr( handle, "vkDestroyDebugUtilsMessengerEXT" );
            destroyMsgCallback( handle, debugMessenger, ALLOCATION_CALLBACK );
        }
    }
} // namespace Engine
