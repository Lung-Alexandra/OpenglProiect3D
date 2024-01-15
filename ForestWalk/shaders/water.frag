#version 330

//	Variabile de intrare (dinspre Shader.vert);
in vec3 ex_Color;

//	Variabile de iesire	(spre programul principal);
out vec3 out_Color;

//  Variabile uniforme;
uniform int codCol;

void main(void)
{

    out_Color=vec3(0.0, 0.0, 0.0);

}