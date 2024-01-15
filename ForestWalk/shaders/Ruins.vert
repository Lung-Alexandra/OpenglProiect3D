
#version 330

//  Variabile de intrare (dinspre programul principal);
layout (location = 0) in vec4 in_Position;     //  Se preia din buffer de pe prima pozitie (0) atributul care contine coordonatele;
layout (location = 1) in vec2 inTex;
layout (location = 2) in mat4 modelMatrix;

//  Variabile de iesire;
out vec4 gl_Position;   //  Transmite pozitia actualizata spre programul principal;
out vec2 TexCoord;
out vec3 FragPos;


//  Variabile uniforme;
uniform mat4 view;
uniform mat4 projection;

void main(void)
{
   gl_Position = projection*view*modelMatrix*in_Position;
   TexCoord = inTex;
   FragPos = in_Position.rgb;
}
