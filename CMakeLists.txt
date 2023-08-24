cmake_minimum_required(VERSION 3.5)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
project("quest"
        LANGUAGES CXX
        VERSION 0.0.0.1
        )

find_package(Vulkan)
find_package(glfw3 PATHS "${CMAKE_SOURCE_DIR}/conan_build")
find_package(spdlog PATHS "${CMAKE_SOURCE_DIR}/conan_build")
find_package(glm PATHS "${CMAKE_SOURCE_DIR}/conan_build")
find_package(tinyobjloader PATHS "${CMAKE_SOURCE_DIR}/conan_build")
find_package(stb PATHS "${CMAKE_SOURCE_DIR}/conan_build")

set(EXECUTABLE_NAME "app")

add_subdirectory(src/assets/models)
add_subdirectory(src/engine)


add_executable(${EXECUTABLE_NAME} "src/main.cxx")
add_dependencies(${EXECUTABLE_NAME} shaders)

target_link_libraries(${EXECUTABLE_NAME} PRIVATE engine::engine)
