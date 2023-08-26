#version 450
// #extension GL_EXT_debug_printf : enable

layout( set = 0, binding = 1 ) uniform sampler2D Sampler;

layout( location = 0 ) in vec4 fragColor;
layout( location = 1 ) in vec2 TextureCord;

layout( location = 0 ) out vec4 outColor;

void main()
{
    outColor = texture( Sampler, TextureCord );
    // outColor = fragColor;
}