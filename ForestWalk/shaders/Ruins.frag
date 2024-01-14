
#version 330

//	Variabile de intrare (dinspre Shader.vert);
in vec2 TexCoord;

//	Variabile de iesire	(spre programul principal);
out vec3 out_Color;

//  Variabile uniforme;
uniform int codCol;

const int numTextures = 1;
uniform sampler2D ruinsTextures[numTextures];


void main(void)
{
  vec4 texColor;
  texColor = texture(ruinsTextures[0], TexCoord);

  out_Color=texColor.rgb;
  
}