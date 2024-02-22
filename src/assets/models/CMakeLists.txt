set(BIN_SHADER_FILE_EXT ".spv")

if (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "AMD64")
        set(GLSL_VALIDATOR "$ENV{VULKAN_SDK}/Bin/glslc.exe")
else()
        set(GLSL_VALIDATOR "$ENV{VULKAN_SDK}/Bin32/glslc.exe")
endif()

file(GLOB_RECURSE GLSL_SOURCE_FILES
        "*/shaders/*"
)
foreach(GLSL ${GLSL_SOURCE_FILES})
    get_filename_component(FILE_NAME ${GLSL} NAME)
    get_filename_component(FILE_DIR ${GLSL} DIRECTORY)
    get_filename_component(FILE_EXT ${GLSL} LAST_EXT)
    set(SPIRV "${FILE_DIR}/${FILE_NAME}${BIN_SHADER_FILE_EXT}")
    if(NOT "${FILE_EXT}" STREQUAL "${BIN_SHADER_FILE_EXT}")
        add_custom_command(
            OUTPUT ${SPIRV}
            COMMAND ${GLSL_VALIDATOR} ${GLSL} -o ${SPIRV}
            DEPENDS ${GLSL})
        list(APPEND SPIRV_BINARY_FILES ${SPIRV})
    endif()
endforeach(GLSL)

add_custom_target(
    shaders 
    DEPENDS ${SPIRV_BINARY_FILES}
)