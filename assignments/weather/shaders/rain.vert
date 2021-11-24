#version 330 core
layout (location = 0) in vec3 pos;
out vec4 vtxColor;

uniform mat4 viewProj;
uniform mat4 viewProjPrev;
uniform vec3 camPos;
uniform vec3 offset;
uniform float boxSize;

void main()
{
    vec3 newPos = mod(pos + offset, 10.0);
    newPos += camPos - boxSize / 2;
    vec3 posPrev = newPos + vec3(0.0, 0.2, 0.0);
    vec4 bottom = viewProj * vec4(newPos, 1.0);
    vec4 top = viewProj * vec4(posPrev, 1.0);
    vec4 topPrev = viewProjPrev * vec4(posPrev, 1.0);

    vec2 dir = top.xy - bottom.xy;
    vec2 dirPrev = topPrev.xy - bottom.xy;
    //gl_PointSize = max(5.0 - distance(newPos, camPos), 1.0);

    float len = length(dir);
    float lenPrev = length(dirPrev);
    //gl_Position = mix(top, bottom, mod(gl_VertexID, 2.0));
    gl_Position = mix(topPrev, bottom, mod(gl_VertexID, 2.0));


    vtxColor = vec4(1.0, 1.0, 1.0, clamp(len / lenPrev * 4.0, 0.2, 0.4));
}