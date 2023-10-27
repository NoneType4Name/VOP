#pragma once
#include <fstream>
#include <optional>
#include <vector>
#include <array>
#include <set>
#include <format>
#include <iostream>
#include <stdint.h>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#define ALLOCATION_CALLBACK nullptr

#define CHECK_RESULT( ret_result )                                                                                                                                        \
    {                                                                                                                                                                     \
        const VkResult result { ret_result };                                                                                                                             \
        if ( result != VK_SUCCESS )                                                                                                                                       \
        {                                                                                                                                                                 \
            SPDLOG_CRITICAL( "Api error in function {}: {}({}); location: {}:{}", #ret_result, string_VkResult( result ), std::to_string( result ), __FILE__, __LINE__ ); \
            assert( result == VK_SUCCESS );                                                                                                                               \
        }                                                                                                                                                                 \
    }
