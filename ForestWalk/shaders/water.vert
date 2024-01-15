
#version 330

//  Variabile de intrare (dinspre programul principal);
layout (location = 0) in vec4 in_Position;     //  Se preia din buffer de pe prima pozitie (0) atributul care contine coordonatele;
layout (location = 1) in vec3 in_Color;        //  Se preia din buffer de pe a doua pozitie (1) atributul care contine culoarea;

//  Variabile de iesire;
out vec4 gl_Position;   //  Transmite pozitia actualizata spre programul principal;
out vec3 ex_Color;      //  Transmite culoarea (de modificat in Shader.frag);

//  Variabile uniforme;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;
uniform float gametime;

void main(void)
{
    vec4 poz = in_Position;
    poz.y = sin(poz.x+poz.z+gametime)*30;

    gl_Position = projection*view*model*poz;
    ex_Color=in_Color;
}
