include(GenerateExportHeader)

set(ENGINE_SOURCE_FILES 
    ./src/engine.cxx
    ./include/engine.hxx
    ./src/common/globals.hxx
    ./src/common/logging.hxx
    ./src/debug.cxx
    ./src/platform.hxx
    )
if(WIN32)
    list(APPEND ENGINE_SOURCE_FILES src/windows/WindowsPlatform.cxx src/windows/WindowsPlatform.hxx src/windows/PlatformDefines.hxx)
    add_definitions(-DPLATFORM_WINDOWS)
endif()


add_library(engine SHARED ${ENGINE_SOURCE_FILES})
add_library(engine::engine ALIAS engine)

find_package(Vulkan)
find_package(glfw3 PATHS "${CMAKE_SOURCE_DIR}/conan_build")
find_package(spdlog PATHS "${CMAKE_SOURCE_DIR}/conan_build")
find_package(glm PATHS "${CMAKE_SOURCE_DIR}/conan_build")
find_package(tinyobjloader PATHS "${CMAKE_SOURCE_DIR}/conan_build")
find_package(stb PATHS "${CMAKE_SOURCE_DIR}/conan_build")

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
