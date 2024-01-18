#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;
//uniform mat4 rotation;
uniform float angle;
mat4 rotateY(float angle)
{
    float s = sin(angle);
    float c = cos(angle);

    return mat4(
    c, 0, -s, 0,
    0, 1, 0, 0,
    s, 0, c, 0,
    0, 0, 0, 1
    );
}
void main()
{
    mat4 rotation = rotateY(angle);
    TexCoords = aPos;
    vec4 pos = projection * view * rotation * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}