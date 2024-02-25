set(BIN_SHADER_FILE_EXT "spv")
set(SHADER_SOURCE_NAME "shader")
set(SHADER_BINARY_NAME "binary")

if (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "AMD64")
        set(GLSL_VALIDATOR "$ENV{VULKAN_SDK}/Bin/glslc.exe")
else()
        set(GLSL_VALIDATOR "$ENV{VULKAN_SDK}/Bin32/glslc.exe")
endif()

file(GLOB_RECURSE GLSL_SOURCE_FILES "${SHADER_SOURCE_NAME}.*")

foreach(GLSL ${GLSL_SOURCE_FILES})
    get_filename_component(FILE_NAME ${GLSL} NAME_WLE)
    get_filename_component(FILE_PATH ${GLSL} DIRECTORY)
    get_filename_component(FILE_EXT ${GLSL} LAST_EXT)
    get_filename_component(SHADER_TYPE ${FILE_NAME} LAST_EXT)
    string(REPLACE "." "" FILE_EXT ${FILE_EXT})
    string(REPLACE "." "" SHADER_TYPE ${SHADER_TYPE})
    set(SPIRV "${CMAKE_SOURCE_DIR}/bin/bin/assets/shaders/${SHADER_BINARY_NAME}.${SHADER_TYPE}.${BIN_SHADER_FILE_EXT}")
        add_custom_command(
            OUTPUT ${SPIRV}
            COMMAND ${GLSL_VALIDATOR} -x ${FILE_EXT} -fshader-stage=${SHADER_TYPE} ${GLSL} -o ${SPIRV} 
            DEPENDS ${GLSL})
        list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(
    shaders 
    DEPENDS ${SPIRV_BINARY_FILES}
)