cmake_minimum_required(VERSION 3.5)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

project("quest"
        LANGUAGES CXX
        VERSION 0.0.0.1
)

find_program(conan_bin conan)
if(NOT EXISTS "${CMAKE_SOURCE_DIR}/bin/conan_build/CMakePresets.json")
        if (NOT EXISTS ${conan_bin})
                message("Conan required. Tryed to install conan use python pip.")
                find_program(pip_bin pip)
                if(NOT EXISTS ${pip_bin})
                        message(FATAL_ERROR "Failed to install conan.\nFailed to find python pip.")  
                execute_process(COMMAND ${pip_bin} install conan)
                endif()
        endif()
        execute_process(COMMAND ${conan_bin} profile detect)
        execute_process(COMMAND ${conan_bin} install . -of=bin/conan_build -b=missing -s build_type=Debug -s compiler.cppstd=20 -s compiler=clang -s compiler.version=16 --conf tools.cmake.cmaketoolchain:generator=Ninja
                        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
                        RESULT_VARIABLE RESULT)
        if (NOT RESULT EQUAL 0)
                message(FATAL_ERROR "Failed to install requires from conan.\n${RESULT}.")
        else()
                message(FATAL_ERROR "Remove cache and Reconfigure.")
        endif()
else()
        execute_process(COMMAND ${conan_bin} install . -of=bin/conan_build -b=missing -s build_type=Debug -s compiler.cppstd=20 -s compiler=clang -s compiler.version=16 --conf tools.cmake.cmaketoolchain:generator=Ninja
                        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
                        RESULT_VARIABLE RESULT)
        if (NOT RESULT EQUAL 0)
                message(FATAL_ERROR "Failed to install requires from conan.\n${RESULT}.")
        endif()
endif()

find_package(Vulkan)
find_package(glfw3 PATHS "${CMAKE_SOURCE_DIR}/conan_build")
find_package(spdlog PATHS "${CMAKE_SOURCE_DIR}/conan_build")
find_package(glm PATHS "${CMAKE_SOURCE_DIR}/conan_build")
find_package(tinyobjloader PATHS "${CMAKE_SOURCE_DIR}/conan_build")
find_package(stb PATHS "${CMAKE_SOURCE_DIR}/conan_build")

set(EXECUTABLE_NAME "app")

add_subdirectory(src/assets/shaders)
add_subdirectory(src/engine)


add_executable(${EXECUTABLE_NAME} "src/main.cxx")
add_dependencies(${EXECUTABLE_NAME} shaders)

target_link_libraries(${EXECUTABLE_NAME} PRIVATE engine::engine)
