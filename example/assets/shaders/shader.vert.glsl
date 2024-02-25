#version 450
// #extension GL_EXT_debug_printf : enable

layout( set = 0, binding = 0 ) uniform UniformBufferObject
{
    mat4 model;
    mat4 view;
    mat4 proj;
}
ubo;

layout( location = 0 ) in vec4 inPosition;
layout( location = 1 ) in vec4 inColor;
layout( location = 2 ) in vec2 inTexture;

layout( location = 0 ) out vec4 fragColor;
layout( location = 1 ) out vec2 fragTexture;

void main()
{
    gl_Position = ubo.proj * ubo.view * ubo.model * inPosition;
    fragColor   = inColor;
    fragTexture = inTexture;
}