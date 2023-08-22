include(GenerateExportHeader)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_library(render SHARED src/render.cxx include/render.hxx)
add_library(render::render ALIAS render)

target_include_directories(render PUBLIC ${glfw3_INCLUDE_DIRS})
target_link_libraries(render glfw)
target_link_libraries(render stb::stb)
target_link_libraries(render Vulkan::Vulkan)
target_link_libraries(render spdlog::spdlog)
target_link_libraries(render tinyobjloader::tinyobjloader)
target_link_libraries(render glm::glm)

target_include_directories(
    render
    PRIVATE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>"
    PUBLIC  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
)

generate_export_header(render EXPORT_FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR}/include/render_export.hxx)
target_compile_definitions(
    render PUBLIC "$<$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>:render_STATIC_DEFINE>")
target_include_directories(
    render PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>")