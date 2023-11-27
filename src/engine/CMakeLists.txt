include(GenerateExportHeader)

option(ENGINE_MODIFY_SUPPORT "Enable support modify engine behavior." ON)

if(${WIN32})
    set(ENGINE_PLATFORM_INCLUDE /src/windows)
endif()
# file(GLOB_RECURSE ENGINE_SOURCE_FILES
# "./src/*.cxx"
# "./${ENGINE_PLATFORM_INCLUDE}/*.cxx"
# )
set(ENGINE_SOURCE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/windows/WindowsPlatform.cxx;${CMAKE_CURRENT_SOURCE_DIR}/src/buffer.cxx;${CMAKE_CURRENT_SOURCE_DIR}/src/device.cxx;${CMAKE_CURRENT_SOURCE_DIR}/src/EHI.cxx;${CMAKE_CURRENT_SOURCE_DIR}/src/engine.cxx;${CMAKE_CURRENT_SOURCE_DIR}/src/image.cxx;${CMAKE_CURRENT_SOURCE_DIR}/src/queue.cxx;${CMAKE_CURRENT_SOURCE_DIR}/src/surface.cxx;${CMAKE_CURRENT_SOURCE_DIR}/src/swapchain.cxx;${CMAKE_CURRENT_SOURCE_DIR}/src/debug.cxx;${CMAKE_CURRENT_SOURCE_DIR}/src/LayersAndExtensions.cxx;${CMAKE_CURRENT_SOURCE_DIR}/src/renderpass.cxx;${CMAKE_CURRENT_SOURCE_DIR}/src/descriptorPool.cxx"
)

add_library(engine SHARED ${ENGINE_SOURCE_FILES} )
add_library(engine::engine ALIAS engine)
if (WIN32)
    set_target_properties(engine PROPERTIES 
                            INTERFACE_COMPILE_DEFINITIONS -DENGINE_PLATFORM_WINDOWS
                            COMPILE_DEFINITIONS -DENGINE_PLATFORM_WINDOWS
    )
endif()

find_package(Vulkan)
find_package(glfw3 PATHS "${CMAKE_SOURCE_DIR}/conan_build")
find_package(spdlog PATHS "${CMAKE_SOURCE_DIR}/conan_build")
find_package(glm PATHS "${CMAKE_SOURCE_DIR}/conan_build")
find_package(tinyobjloader PATHS "${CMAKE_SOURCE_DIR}/conan_build")
find_package(stb PATHS "${CMAKE_SOURCE_DIR}/conan_build")

target_include_directories(engine PRIVATE ${glfw3_INCLUDE_DIRS})
target_link_libraries(engine glfw)
target_link_libraries(engine stb::stb)
target_link_libraries(engine Vulkan::Vulkan)
target_link_libraries(engine spdlog::spdlog)
target_link_libraries(engine tinyobjloader::tinyobjloader)
target_link_libraries(engine glm::glm)
if (${ENGINE_MODIFY_SUPPORT})
    target_include_directories(
        engine
        PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>"
        PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    )
else()
    target_include_directories(
        engine
        PRIVATE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>"
        PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        )
endif()

generate_export_header(engine EXPORT_FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR}/include/engine_export.hxx)
target_compile_definitions(engine PUBLIC "$<$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>:engine_STATIC_DEFINE>")