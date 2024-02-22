#pragma once
#ifndef GLOBALS_HXX
#    define GLOBALS_HXX
#    include <set>
#    include <array>
#    include <format>
#    include <memory>
#    include <vector>
#    include <fstream>
#    include <optional>
#    include <iostream>
#    include <algorithm>
#    include <stdint.h>
#    include <unordered_map>
#    include <ObjectiveVulkan/ObjectiveVulkan.hxx>
#    define OBJECTIVE_VULKAN_ALLOCATION_CALLBACK nullptr
#    define OBJECTIVE_VULKAN_OBJECTIVE_VULKAN_DEFINE_DATA_FIELD( ... ) \
        const_cast<std::unique_ptr<OBJECTIVE_VULKAN_DATA_TYPE> &>( data ) = std::make_unique<OBJECTIVE_VULKAN_DATA_TYPE>( this, __VA_ARGS__ );
#    define OBJECTIVE_VULKAN_CHECK_RESULT( ret_result )                                                                                                                       \
        {                                                                                                                                                                     \
            const VkResult result { ret_result };                                                                                                                             \
            if ( result != VK_SUCCESS )                                                                                                                                       \
            {                                                                                                                                                                 \
                SPDLOG_CRITICAL( "Api error in function {}: {}({}); location: {}:{}", #ret_result, string_VkResult( result ), std::to_string( result ), __FILE__, __LINE__ ); \
                assert( result == VK_SUCCESS );                                                                                                                               \
            }                                                                                                                                                                 \
        }
#endif