#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 rotation;

void main()
{
    TexCoords = aPos;
    vec4 pos = projection * view * rotation * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}