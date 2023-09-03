#pragma once
#include <fstream>
#include <set>
#include <array>
#include <vector>
#include <map>
#include <format>
#include <stdint.h>
#include <common/logging.hxx>

#define CHECK_RESULT( ret_result )                                                                                                                                        \
    {                                                                                                                                                                     \
        const VkResult result{ ret_result };                                                                                                                              \
        if( result != VK_SUCCESS )                                                                                                                                        \
        {                                                                                                                                                                 \
            SPDLOG_CRITICAL( "Api error in function {}: {}({}); location:{}:{}.", #ret_result, string_VkResult( result ), std::to_string( result ), __FILE__, __LINE__ ); \
            assert( result == VK_SUCCESS );                                                                                                                               \
        }                                                                                                                                                                 \
    }
