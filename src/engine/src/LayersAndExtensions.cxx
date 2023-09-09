#include <platform.hxx>
#include <RHI.hxx>

namespace Engine
{
    namespace tools
    {
        std::vector<std::string> instanceLayers{
#ifdef PLATFORM_WINDOWS
            "VK_LAYER_KHRONOS_validation"
#endif
        };
        std::vector<std::string> instanceExtensions{
            VK_EXT_DEBUG_UTILS_EXTENSION_NAME
#ifdef PLATFORM_WINDOWS
#endif
        };
        std::vector<std::string> deviceExtensions{
#ifdef PLATFORM_WINDOWS
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
#endif
        };

        void getDeviceExtensions( std::vector<const char *> &deviceExtensions )
        {
            for( size_t i{ 0 }; tools::deviceExtensions.size() > i; i++ )
            {
                deviceExtensions.push_back( tools::deviceExtensions[ i ].data() );
            }
        }

        void getInstanceExtensions( std::vector<const char *> &instanceExtensions )
        {
            for( size_t i{ 0 }; tools::instanceExtensions.size() > i; i++ )
            {
                instanceExtensions.push_back( tools::instanceExtensions[ i ].data() );
            }
            uint32_t _c{ 0 };
            const char **req_exts{ glfwGetRequiredInstanceExtensions( &_c ) };
            for( size_t i{ 0 }; _c > i; i++ )
            {
                instanceExtensions.push_back( req_exts[ i ] );
            }
        }

        void getLayers( std::vector<const char *> &layers )
        { // #ifdef ENGINE_DEBUG
            for( size_t i{ 0 }; tools::instanceLayers.size() > i; i++ )
            {
                layers.push_back( tools::instanceLayers[ i ].data() );
            }
            // #endif
        } // namespace tools
    }     // namespace tools
} // namespace Engine