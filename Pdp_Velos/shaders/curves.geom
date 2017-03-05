#version 330 core

layout (lines) in;
layout (lines) out;


vec2 quadraticBezierCurve(vec2 p0, vec2 p1, vec2 p2, float t)
{
    return (1.f - t) * (1.f - t) * p0 +
            2 * t * (1.f - t) p1 +
            t * t * p2;
}


void main()
{
    vec2 p1 = gl_PositionIn[0].xy;
    vec2 p2 = gl_PositionIn[1].xy;

    float dist = distance(p1 - p2);

    int it = ceil(dist);

    for (int i = 0; i < it; ++)
    {
        vec2 p = quadraticBezierCurve(p1, p2 - p1, p2, i);
        gl_Position = p;
        EmitVertex();
    }
    EndPrimitive();
}
