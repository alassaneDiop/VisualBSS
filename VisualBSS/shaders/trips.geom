#version 330 core

layout (lines) in;
layout (line_strip, max_vertices = 50) out;

in vec3 vColor[2];
out vec3 gColor;

// Nomber of points generated, more there is more the curve is smooth
int generatedPointsNb = 20;

vec3 evalQuadraticBezier(vec3 v[3], float t)
{
    float oneMinusT = 1.0 - t;

    float b0 = oneMinusT * oneMinusT;
    float b1 = 2.0 * oneMinusT * t;
    float b2 = t * t;

    return b0 * v[0] + b1 * v[1] + b2 * v[2];
}

void main()
{
    // The position of the point on the middle of the segment
    vec3 midLinePos = (gl_in[0].gl_Position.xyz + gl_in[1].gl_Position.xyz) / 2.f;

    // Calculate control point (wrong way to calculate CP)
    vec3 posCP;
    posCP.x = midLinePos.x - (gl_in[1].gl_Position.y - gl_in[0].gl_Position.y);
    posCP.y = midLinePos.y;

/// A
/// |
/// |-C   distance from segment is dist(A, B) / 2
/// |
/// B

//    float dist = distance(gl_in[0].gl_Position.xyz,  gl_in[1].gl_Position.xyz);
//    float z = dist * sqrt(13.f) / 6.f;
//    float angle = atan(2.f / 3.f);

//    posCP.x = gl_in[0].gl_Position.x + z * cos(angle);
//    posCP.y = gl_in[0].gl_Position.y + z * sin(angle);

    vec3 pos[3];
    pos[0] = gl_in[0].gl_Position.xyz;
    pos[1] = posCP;
    pos[2] = gl_in[1].gl_Position.xyz;

    float pointsInterval = 1.0 / float(generatedPointsNb - 1.0);

    for(int i = 0; i < generatedPointsNb; i++)
    {
        float t = i * pointsInterval;
        vec3 p = evalQuadraticBezier(pos, t);

        gl_Position = vec4(p.xyz, 1.0);

        // color interpolation
        gColor = mix(vColor[0], vColor[1], t);

        EmitVertex();
    }

    EndPrimitive();
}
