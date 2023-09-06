#pragma once
#include <fstream>

#define ALLOCATION_CALLBACK nullptr

#define CHECK_RESULT( ret_result )                                                                                                                                        \
    {                                                                                                                                                                     \
        const VkResult result{ ret_result };                                                                                                                              \
        if( result != VK_SUCCESS )                                                                                                                                        \
        {                                                                                                                                                                 \
            SPDLOG_CRITICAL( "Api error in function {}: {}({}); location:{}:{}.", #ret_result, string_VkResult( result ), std::to_string( result ), __FILE__, __LINE__ ); \
            assert( result == VK_SUCCESS );                                                                                                                               \
        }                                                                                                                                                                 \
    }
