file(GLOB_RECURSE OBJECTIVE_VULKAN_SOURCE_FILES "./*.cxx")

add_library(ObjectiveVulkan ${OBJECTIVE_VULKAN_SOURCE_FILES})

find_package(Vulkan)
find_package(spdlog)

target_link_libraries(ObjectiveVulkan Vulkan::Vulkan)
target_link_libraries(ObjectiveVulkan spdlog::spdlog)

target_include_directories(ObjectiveVulkan PUBLIC "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>")

generate_export_header(ObjectiveVulkan EXPORT_FILE_NAME ${CMAKE_SOURCE_DIR}/include/ObjectiveVulkan/ObjectiveVulkan_export.hxx)
if(NOT ${BUILD_SHARED_LIBS})
    target_compile_definitions(ObjectiveVulkan PUBLIC -DOBJECTIVEVULKAN_STATIC_DEFINE)
endif()