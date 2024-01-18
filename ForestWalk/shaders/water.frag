#version 330

in vec3 Normal;
in vec3 Position;
uniform float gametime;
uniform vec3 cameraPos;
uniform samplerCube skyboxday;
uniform samplerCube skyboxnight;

//	Variabile de iesire	(spre programul principal);
out vec4 out_Color;

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
    vec3 I = normalize(Position - cameraPos);
    vec3 R = reflect(I, normalize(Normal));

    mat4 rotation = rotateY(gametime* 0.02f);
    R = (rotation * vec4(R, 1.0)).xyz;

    vec4 texture1 = texture(skyboxday, R);
    vec4 texture2 = texture(skyboxnight, R);
    texture1.w = 0.5;
    texture2.w = 0.5;

    out_Color =mix(texture2, texture1, smoothstep(-0.5, 0.5, sin(gametime*0.09)));

}