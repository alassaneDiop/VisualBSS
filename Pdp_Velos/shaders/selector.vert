#version 330 core

layout(location = 0) in vec2 vtx_position;

uniform float translation;

void main()
{
    gl_Position = vec4(vtx_position.x,
                       vtx_position.y,
                       0.f, 1.f);
}
