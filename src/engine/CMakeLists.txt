include(GenerateExportHeader)

add_library(engine SHARED src/engine.cxx src/vulkanTools.hxx include/engine.hxx)
add_library(engine::engine ALIAS engine)

target_include_directories(engine PUBLIC ${glfw3_INCLUDE_DIRS})
target_link_libraries(engine glfw)
target_link_libraries(engine stb::stb)
target_link_libraries(engine Vulkan::Vulkan)
target_link_libraries(engine spdlog::spdlog)
target_link_libraries(engine tinyobjloader::tinyobjloader)
target_link_libraries(engine glm::glm)

target_include_directories(
    engine
    PRIVATE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>"
    PUBLIC  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
)

generate_export_header(engine EXPORT_FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR}/include/engine_export.hxx)
target_compile_definitions(
    engine PUBLIC "$<$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>:engine_STATIC_DEFINE>")
target_include_directories(
    engine PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>")
