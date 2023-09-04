#include <platform.hxx>
#include <device.hxx>
#include <RHI.hxx>

namespace Engine
{
    namespace
    {
        namespace tools
        {
            namespace
            {
                static const char *_layersInstance[]{
#ifdef PLATFORM_WINDOWS
                    "VK_LAYER_KHRONOS_validation"
#endif
                };
                static const char *_extensionsInstance[]{
                    VK_EXT_DEBUG_UTILS_EXTENSION_NAME
#ifdef PLATFORM_WINDOWS
#endif
                };
                static const char *_layersDevice[]{
#ifdef PLATFORM_WINDOWS
                    "VK_LAYER_KHRONOS_validation"
#endif
                };
                static const char *_extensionsDevice[]{
#ifdef PLATFORM_WINDOWS
                    "VK_KHR_SWAPCHAIN_EXTENSION_NAME"
#endif
                };

            } // namespace
            static void getDeviceLayersAndExtension( std::vector<const char *> &deviceLayers, std::vector<const char *> &deviceExtensions )
            {
#ifdef ENGINE_DEBUG
                for( size_t i{ 0 }; _layersDevice[ i ] != nullptr; i++ )
                {
                    deviceLayers.push_back( _layersDevice[ i ] );
                }
#endif
                for( size_t i{ 0 }; _extensionsDevice[ i ] != nullptr; i++ )
                {
                    deviceExtensions.push_back( _extensionsDevice[ i ] );
                }
            }

            static void getInstanceLayersAndExtension( std::vector<const char *> &instanceLayers, std::vector<const char *> &instanceExtensions )
            {
#ifdef ENGINE_DEBUG
                for( size_t i{ 0 }; _layersInstance[ i ] != nullptr; i++ )
                {
                    instanceLayers.push_back( _layersInstance[ i ] );
                }
#endif

                for( size_t i{ 0 }; _extensionsInstance[ i ] != nullptr; i++ )
                {
                    instanceLayers.push_back( _extensionsInstance[ i ] );
                }
                uint32_t _c{ 0 };
                const char **req_exts{ glfwGetRequiredInstanceExtensions( &_c ) };
                for( size_t i{ 0 }; i < _c; i++ )
                {
                    instanceLayers.push_back( req_exts[ i ] );
                }
            }
        } // namespace tools

    } // namespace

} // namespace Engine
