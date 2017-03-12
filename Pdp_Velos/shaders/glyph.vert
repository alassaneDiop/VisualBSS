#version 330 core

layout(location = 0) in vec2 vtx_position;
layout(location = 1) in vec3 vtx_color;

uniform float translation;

out vec3 color;

void main()
{
    gl_Position = vec4(vtx_position.x,
                       vtx_position.y - translation,
                       0.f, 1.f);

    color = vtx_color;
}
