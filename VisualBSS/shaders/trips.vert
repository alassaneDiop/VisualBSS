#version 330 core

layout(location = 0) in vec2 vtx_position;
layout(location = 1) in vec3 vtx_color;

uniform vec2 translation;
uniform float zoom;

out vec3 vColor;

void main()
{
    gl_Position = vec4(zoom * (vtx_position.x + translation.x),
                       zoom * (vtx_position.y - translation.y),
                       0.f, 1.f);

    vColor = vtx_color;
}
