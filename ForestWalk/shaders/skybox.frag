#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skyboxday;
uniform samplerCube skyboxnight;
uniform float gametime;

void main()
{
    vec4 texture1 = texture(skyboxday, TexCoords);
    vec4 texture2 = texture(skyboxnight, TexCoords);

    FragColor = mix(texture2, texture1,smoothstep(-0.5, 0.5, sin(gametime*0.09)));


}