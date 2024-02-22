#include <instance.hxx>
#include <common/logging.hxx>

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
            switch ( MessageLevel )
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
                default:
                    break;
            }
            return VK_FALSE;
        }
    } // namespace

    VkDebugUtilsMessengerCreateInfoEXT instance::OBJECTIVE_VULKAN_DATA_TYPE::setupDebugLayerCallback()
    {
        VkDebugUtilsMessengerCreateInfoEXT createInfo {};
        createInfo.sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType     = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = DebugCallback;
        createInfo.pUserData       = this;
        return createInfo;
    }

    void instance::OBJECTIVE_VULKAN_DATA_TYPE::initDebugLayerCallBack( VkDebugUtilsMessengerCreateInfoEXT createInfo )
    {
        auto createMsgCallback = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>( vkGetInstanceProcAddr( parent->handle, "vkCreateDebugUtilsMessengerEXT" ) );
        if ( createMsgCallback != nullptr )
        {
            OBJECTIVE_VULKAN_CHECK_RESULT( createMsgCallback( parent->handle, &createInfo, OBJECTIVE_VULKAN_ALLOCATION_CALLBACK, &debugMessenger ) );
        }
        else
        {
            SPDLOG_CRITICAL( "Failed to get Debug messenger addres." );
        }
    }

    void instance::OBJECTIVE_VULKAN_DATA_TYPE::destroyDebugLayerCallback()
    {
        if ( debugMessenger != VK_NULL_HANDLE )
        {
            PFN_vkDestroyDebugUtilsMessengerEXT destroyMsgCallback = ( PFN_vkDestroyDebugUtilsMessengerEXT ) vkGetInstanceProcAddr( parent->handle, "vkDestroyDebugUtilsMessengerEXT" );
            destroyMsgCallback( parent->handle, debugMessenger, OBJECTIVE_VULKAN_ALLOCATION_CALLBACK );
        }
    }
} // namespace Engine
