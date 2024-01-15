
#version 330

//	Variabile de intrare (dinspre Shader.vert);
in vec2 TexCoord;
in vec3 FragPos;

//	Variabile de iesire	(spre programul principal);
out vec4 out_Color;

//  Variabile uniforme;
uniform int codCol;

uniform vec3 LightPos;
uniform float gametime;

const int numTextures = 2;
uniform sampler2D ruinsTextures[numTextures];


void main(void)
{
  vec4 texColor;
  texColor = texture(ruinsTextures[0], TexCoord);

  vec3 normal;
  normal = texture(ruinsTextures[1], TexCoord).rgb;

	float diffuseFactor = dot(normal, normalize(LightPos - FragPos));
	diffuseFactor = max(diffuseFactor, 0.3f);
	float nightDarknessFactor = 0.7f;

	// Multiply the diffuse factor by the night darkness factor during the night
	diffuseFactor *= mix(nightDarknessFactor, 1.0, smoothstep(-0.5, 0.5, sin(gametime*0.09)));

	vec4 dayAmbientLight = vec4(0.12f, 0.12f, 0.12f, 0.0f);
	vec4 nightAmbientLight = vec4(0.04f, 0.04f, 0.04f, 0.0f);
	vec4 ambientLight = mix(nightAmbientLight, dayAmbientLight, smoothstep(-0.5, 0.5, sin(gametime*0.09)));

  out_Color=ambientLight +  texColor * diffuseFactor;
  
}