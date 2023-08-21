cmake_minimum_required(VERSION 3.5)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
project("quest"
        LANGUAGES CXX
        VERSION 0.0.0.1
        )

set(EXECUTABLE_NAME "window")

add_executable(${EXECUTABLE_NAME} "src/main.cxx")
