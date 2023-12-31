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
#    define GLM_FORCE_RADIANS
#    define GLM_FORCE_DEPTH_ZERO_TO_ONE
#    include <glm/glm.hpp>
#    include <glm/gtc/matrix_transform.hpp>
#    include <glm/gtx/hash.hpp>
#    include <engine.hxx>

#    define ALLOCATION_CALLBACK nullptr
#    define DEFINE_DATA_FIELD \
        const_cast<std::unique_ptr<DATA_TYPE> &>( data ) = std::make_unique<DATA_TYPE>( this );
#    define CHECK_RESULT( ret_result )                                                                                                                                        \
        {                                                                                                                                                                     \
            const VkResult result { ret_result };                                                                                                                             \
            if ( result != VK_SUCCESS )                                                                                                                                       \
            {                                                                                                                                                                 \
                SPDLOG_CRITICAL( "Api error in function {}: {}({}); location: {}:{}", #ret_result, string_VkResult( result ), std::to_string( result ), __FILE__, __LINE__ ); \
                assert( result == VK_SUCCESS );                                                                                                                               \
            }                                                                                                                                                                 \
        }

// template<typename _T>
// struct cmp
// {
//     using is_transparent = void;
//     bool operator()( const _T *a, const _T *b ) const
//     {
//         return a < b;
//     }
// };

#endif