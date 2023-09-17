cmake_minimum_required(VERSION 3.5)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/app)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/app)

project("quest"
        LANGUAGES CXX
        VERSION 0.0.0.1
)

set(EXECUTABLE_NAME "app")

add_subdirectory(src/assets/shaders)
add_subdirectory(src/engine)
file(COPY ${CMAKE_SOURCE_DIR}/src/assets/models DESTINATION ${CMAKE_SOURCE_DIR}/bin/app/assets/)
file(COPY ${CMAKE_SOURCE_DIR}/src/assets/textures DESTINATION ${CMAKE_SOURCE_DIR}/bin/app/assets/)


add_executable(${EXECUTABLE_NAME} "src/main.cxx")
add_dependencies(${EXECUTABLE_NAME} shaders)

target_link_libraries(${EXECUTABLE_NAME} PRIVATE engine::engine)
