#version 300 es

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec4 aColor;
layout(location = 2) in vec2 aTexCoords;

out vec4 vColor;
out vec2 vTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
in vec2 texCenter;

void main()
{
    vColor = aColor;
    vTexCoords = aTexCoords + texCenter;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
