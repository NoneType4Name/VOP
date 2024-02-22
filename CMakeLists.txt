cmake_minimum_required(VERSION 3.5)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/lib)
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}; ${CMAKE_BINARY_DIR}")
set(CMAKE_BUILD_TYPE Debug)
set(BUILD_SHARED_LIBS OFF)

project("QuesGame"
        LANGUAGES CXX
        VERSION 1.0.0
)

set(EXECUTABLE_NAME "app")
add_subdirectory(src/assets/shaders)
add_subdirectory(dependencies/ObjectiveVulkan)
file(COPY ${CMAKE_SOURCE_DIR}/src/assets/models DESTINATION ${CMAKE_SOURCE_DIR}/bin/bin/assets/)
file(COPY ${CMAKE_SOURCE_DIR}/src/assets/textures DESTINATION ${CMAKE_SOURCE_DIR}/bin/bin/assets/)

find_package(glfw3)
find_package(glm)
find_package(spdlog)
find_package(stb)
find_package(tinyobjloader)

add_executable(${EXECUTABLE_NAME} "src/main.cxx")
add_dependencies(${EXECUTABLE_NAME} shaders)

target_link_libraries(${EXECUTABLE_NAME} stb::stb)
target_link_libraries(${EXECUTABLE_NAME} ObjectiveVulkan::ObjectiveVulkan)
target_link_libraries(${EXECUTABLE_NAME} tinyobjloader::tinyobjloader)
target_link_libraries(${EXECUTABLE_NAME} glfw)
