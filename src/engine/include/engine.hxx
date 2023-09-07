#pragma once
#ifndef ENGINE_HXX
#    define ENGINE_HXX
#    define TINYOBJLOADER_IMPLEMENTATION
#    define GLM_FORCE_RADIANS
#    define GLM_FORCE_DEPTH_ZERO_TO_ONE
#    define STB_IMAGE_IMPLEMENTATION
#    ifndef DEBUG
#        ifdef _DEBUG
#            define DEBUG               true
#            define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#        else
#            define DEBUG               false
#            define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_CRITICAL
#        endif
#    endif
#    include <set>
#    include <array>
#    include <string>
#    include <vector>
#    include <stdint.h>
#    include "engine_export.hxx"

namespace Engine
{
    struct ENGINE_EXPORT Settings
    {
        uint32_t MultiSamplingCount;
    };

    // struct ENGINE_EXPORT AppModels
    // {
    //     inline uint32_t add( std::array<const char *, 2> &&_model_texture )
    //     {
    //         models_texture.push_back( _model_texture );
    //         return _model_texture.size();
    //     };

    //     std::vector<std::array<const char *, 2>> get()
    //     {
    //         return models_texture;
    //     }

    //   private:
    //     std::vector<std::array<const char *, 2>> models_texture;
    // };

    struct ENGINE_EXPORT AppCreateInfo
    {
        uint16_t width{ 800 };
        uint16_t height{ 600 };
        const char *title{};
        std::vector<std::array<const char *, 2>> vAppModels{};
        const char *VertexShaderPath;
        const char *FragmentShaderPath;
        Settings sSettings{};
    };
    ENGINE_EXPORT void setup();
    ENGINE_EXPORT void init( AppCreateInfo sAppCreateInfo );
    // ENGINE_EXPORT void SetGraphicDevice( GrapchicPhysicalDevice device );
    // ENGINE_EXPORT GrapchicPhysicalDevice GetActiveGrapchiDevice();
    // ENGINE_EXPORT std::vector<GrapchicPhysicalDevice> GetGraphicDevices();
    ENGINE_EXPORT void shutdown();

} // namespace Engine

#endif
