#version 330

in vec2 TexCoord;
in vec3 FragPos;
in mat3 TBN;
in vec3 rec_normal;

out vec4 out_Color;

uniform vec3 LightPos;
uniform vec3 viewPos;
uniform float gametime;
uniform sampler2D treesTextures[2];

void main(void)
{
    // Texture color
    vec4 texColor = texture(treesTextures[0], TexCoord);

    // Normal map
    vec3 normalMap = texture(treesTextures[0], TexCoord).rgb;
    normalMap = vec3(0.0, 0.0, 1.0);
    normalMap = normalize(normalMap * 2.0 - 1.0); // Convert from [0, 1] to [-1, 1]
    vec3 normal = normalize(TBN * normalMap); // Transform to world space

    // Combine normals
    vec3 finalNormal = normal;

    // Lighting calculations
    float diffuseFactor = dot(finalNormal, normalize(LightPos - FragPos));
    diffuseFactor = max(diffuseFactor, 0.3f);
    float nightDarknessFactor = 0.7f;

    // Night lighting adjustments
    diffuseFactor *= mix(nightDarknessFactor, 1.0, smoothstep(-0.5, 0.5, sin(gametime*0.09)));

    // Ambient light
    vec4 dayAmbientLight = vec4(0.12f, 0.12f, 0.12f, 0.0f);
    vec4 nightAmbientLight = vec4(0.04f, 0.04f, 0.04f, 0.0f);
    vec4 ambientLight = mix(nightAmbientLight, dayAmbientLight, smoothstep(-0.5, 0.5, sin(gametime*0.09)));

    // Combine color output
    out_Color = ambientLight + texColor * diffuseFactor;

}