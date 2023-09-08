#include <platform.hxx>
#include <RHI.hxx>

namespace Engine
{
    namespace tools
    {
        namespace
        {
            const std::vector<std::string> _layersInstance{
#ifdef PLATFORM_WINDOWS
#endif
            };
            const std::vector<std::string> _extensionsInstance{
                VK_EXT_DEBUG_UTILS_EXTENSION_NAME
#ifdef PLATFORM_WINDOWS
#endif
            };
            const std::vector<std::string> _layersDevice{
#ifdef PLATFORM_WINDOWS
                "VK_LAYER_KHRONOS_validation"
#endif
            };
            const std::vector<std::string> _extensionsDevice{
#ifdef PLATFORM_WINDOWS
                VK_KHR_SWAPCHAIN_EXTENSION_NAME
#endif
            };

        } // namespace
        static void getDeviceLayersAndExtension( std::vector<const char *> &deviceLayers, std::vector<const char *> &deviceExtensions )
        {
            // #ifdef ENGINE_DEBUG
            for( size_t i{ 0 }; _layersDevice.size() > i; i++ )
            {
                deviceLayers.push_back( _layersDevice[ i ].data() );
            }
            // #endif
            for( size_t i{ 0 }; _extensionsDevice.size() > i; i++ )
            {
                deviceExtensions.push_back( _extensionsDevice[ i ].data() );
            }
        }

        static void getInstanceLayersAndExtension( std::vector<const char *> &instanceLayers, std::vector<const char *> &instanceExtensions )
        {
            // #ifdef ENGINE_DEBUG
            for( size_t i{ 0 }; _layersInstance.size() > i; i++ )
            {
                instanceLayers.push_back( _layersInstance[ i ].data() );
            }
            // #endif

            for( size_t i{ 0 }; _extensionsInstance.size() > i; i++ )
            {
                instanceLayers.push_back( _extensionsInstance[ i ].data() );
            }
            uint32_t _c{ 0 };
            const char **req_exts{ glfwGetRequiredInstanceExtensions( &_c ) };
            for( size_t i{ 0 }; _c > i; i++ )
            {
                instanceLayers.push_back( req_exts[ i ] );
            }
        }
    } // namespace tools
} // namespace Engine
