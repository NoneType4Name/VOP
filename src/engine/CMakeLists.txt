include(GenerateExportHeader)

file(GLOB_RECURSE ENGINE_SOURCE_FILES "./src/*.cxx")

add_library(engine SHARED ${ENGINE_SOURCE_FILES})
add_library(engine::engine ALIAS engine)

if(WIN32)
    add_compile_definitions(engine VK_USE_PLATFORM_WIN32_KHR)
endif()

find_package(Vulkan)
find_package(spdlog PATHS "${CMAKE_SOURCE_DIR}/bin/conan_build")

target_link_libraries(engine Vulkan::Vulkan)
target_link_libraries(engine spdlog::spdlog)

target_include_directories(
    engine
    PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
)

generate_export_header(engine EXPORT_FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR}/include/engine_export.hxx)
target_compile_definitions(engine PUBLIC "$<$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>:engine_STATIC_DEFINE>")