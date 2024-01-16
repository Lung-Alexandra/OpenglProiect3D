#version 330

in vec3 Normal;
//in vec4 FragColor;
in vec3 Position;
uniform float gametime;
//
uniform vec3 cameraPos;
uniform samplerCube skyboxday;
uniform samplerCube skyboxnight;

//	Variabile de iesire	(spre programul principal);
out vec4 out_Color;


void main()
{
    vec3 I = normalize(Position - cameraPos);
    vec3 R = reflect(I, normalize(Normal));
    vec4 texture1 = texture(skyboxday, R);
    texture1.a = 1;
    vec4 texture2 = texture(skyboxnight, R);
    texture2.a = 1;

    out_Color = mix(vec4(0.799, 0.940, 0.919, 1), mix(texture2, texture1, smoothstep(-0.5, 0.5, sin(gametime*0.09))), 0.9);
}