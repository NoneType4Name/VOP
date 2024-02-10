include(GenerateExportHeader)

option(ENGINE_MODIFY_SUPPORT "Enable support modify engine behavior." OFF)

if(${WIN32})
    set(ENGINE_PLATFORM_INCLUDE /src/windows)
endif()

file(GLOB_RECURSE ENGINE_SOURCE_FILES
    "./src/*.cxx"
    "./${ENGINE_PLATFORM_INCLUDE}/*.cxx"
)

add_library(engine SHARED ${ENGINE_SOURCE_FILES})
add_library(engine::engine ALIAS engine)

if(WIN32)
    add_compile_definitions(engine VK_USE_PLATFORM_WIN32_KHR)
    # set_target_properties(engine PROPERTIES
    #     INTERFACE_COMPILE_DEFINITIONS -DENGINE_PLATFORM_WINDOWS
    #     COMPILE_DEFINITIONS -DVK_USE...
    # )
endif()

find_package(Vulkan)
find_package(spdlog PATHS "${CMAKE_SOURCE_DIR}/bin/conan_build")

target_link_libraries(engine Vulkan::Vulkan)
target_link_libraries(engine spdlog::spdlog)

target_include_directories(
    engine
    PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>"
    PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
)

generate_export_header(engine EXPORT_FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR}/include/engine_export.hxx)
target_compile_definitions(engine PUBLIC "$<$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>:engine_STATIC_DEFINE>")