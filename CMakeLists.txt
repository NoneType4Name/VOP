cmake_minimum_required(VERSION 3.5)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
project("quest"
        LANGUAGES CXX
        VERSION 0.0.0.1
        )

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
find_package(spdlog REQUIRED)
find_package(glm)
find_package(tinyobjloader)
find_package(stb)

set(EXECUTABLE_NAME "app")
add_subdirectory(src/assets/models)
add_executable(${EXECUTABLE_NAME} "src/main.cxx")
add_dependencies(${EXECUTABLE_NAME} shaders)
