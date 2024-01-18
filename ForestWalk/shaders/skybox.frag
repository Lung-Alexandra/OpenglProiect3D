#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skyboxday;
uniform samplerCube skyboxnight;
uniform float gametime;
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
    vec3 TexCoords = (rotation * vec4(TexCoords,1.0)).xyz;

    vec4 texture1 = texture(skyboxday, TexCoords);
    vec4 texture2 = texture(skyboxnight, TexCoords);

    FragColor = mix(texture2, texture1,smoothstep(-0.5, 0.5, sin(gametime*0.09)));
}