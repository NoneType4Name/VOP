link_libraries(ObjectiveVulkan)

set_target_properties(ObjectiveVulkan PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/bin)
set_target_properties(ObjectiveVulkan PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/lib)
set_target_properties(ObjectiveVulkan PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/lib)

set(EXECUTABLE_NAME "example")
add_subdirectory(assets/shaders)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets/models DESTINATION ${CMAKE_SOURCE_DIR}/bin/bin/assets/)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets/textures DESTINATION ${CMAKE_SOURCE_DIR}/bin/bin/assets/)

find_package(glfw3)
find_package(glm)
find_package(spdlog)
find_package(stb)
find_package(tinyobjloader)

add_executable(${EXECUTABLE_NAME} "main.cxx")
add_dependencies(${EXECUTABLE_NAME} shaders)

target_link_libraries(${EXECUTABLE_NAME} stb::stb)
target_link_libraries(${EXECUTABLE_NAME} ObjectiveVulkan)
target_link_libraries(${EXECUTABLE_NAME} tinyobjloader::tinyobjloader)
target_link_libraries(${EXECUTABLE_NAME} glfw)

set_target_properties(${EXECUTABLE_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/bin)
set_target_properties(${EXECUTABLE_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/lib)
set_target_properties(${EXECUTABLE_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/lib)