#version 330 core

layout (lines) in;
layout (line_strip, max_vertices = 50) out;

in vec3 vColor[2];
out vec3 gColor;

int generatedPoints = 25;

vec3 evalBezierPos(vec3 v[3], float t)
{
    float oneMinusT = 1.0 - t;

    float b0 = oneMinusT * oneMinusT;
    float b1 = 2.0 * oneMinusT * t;
    float b2 = t * t;

    return b0 * v[0] + b1 * v[1] + b2 * v[2];
}

void main()
{
    vec3 tierOfTheLine = (gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz) / 3;

    vec3 midLinePos = gl_in[0].gl_Position.xyz +
                (abs(gl_in[0].gl_Position.xyz - gl_in[1].gl_Position.xyz) / 2);

    vec3 posCP;
    posCP.x = midLinePos.x + tierOfTheLine.y;
    posCP.y = midLinePos.y + tierOfTheLine.x;

    vec3 pos[3];
    pos[0] = gl_in[0].gl_Position.xyz;
    pos[1] = posCP;
    pos[2] = gl_in[1].gl_Position.xyz;

    float pointsInterval = 1.0 / float(generatedPoints - 1.0);

    for(int i = 0; i < generatedPoints; i++)
    {
        float t = i * pointsInterval;
        vec3 p = evalBezierPos(pos, t);

        gl_Position = vec4(p.xyz, 1.0);
        gColor = mix(vColor[0], vColor[1], t);

        EmitVertex();
    }

    EndPrimitive();
}
