cmake_minimum_required(VERSION 3.5)
project("Vulkan Objective Project"
        LANGUAGES CXX
        VERSION 1.0
)
include(GenerateExportHeader)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE OBJECTIVE_VULKAN_SOURCE_FILES "./src/*.cxx")

add_library(ObjectiveVulkan ${OBJECTIVE_VULKAN_SOURCE_FILES})
add_library(ObjectiveVulkan::ObjectiveVulkan ALIAS ObjectiveVulkan)


find_package(Vulkan)
find_package(spdlog)

target_link_libraries(ObjectiveVulkan Vulkan::Vulkan)
target_link_libraries(ObjectiveVulkan spdlog::spdlog)

target_include_directories(ObjectiveVulkan PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>")  
    
generate_export_header(ObjectiveVulkan EXPORT_FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR}/include/ObjectiveVulkan/ObjectiveVulkan_export.hxx)
if(NOT ${BUILD_SHARED_LIBS})
    target_compile_definitions(ObjectiveVulkan PUBLIC -DOBJECTIVEVULKAN_STATIC_DEFINE)
endif()