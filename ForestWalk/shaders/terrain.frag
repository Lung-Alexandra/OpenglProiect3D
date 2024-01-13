#version 330 core

layout (location = 0) out vec4 FragColor;

in vec4 Color;
in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

uniform vec3 LightPos;

uniform float gametime;
uniform float MIN_Y;
uniform float MAX_Y;
float delta = (MAX_Y - MIN_Y) / 6;

const int numTextures = 6;
float heights[] = {MIN_Y + 0.1 * delta, MIN_Y + 0.2 * delta, MIN_Y + 0.4 * delta, MIN_Y + 0.6 * delta, MIN_Y + 2 * delta, MIN_Y + 4 * delta};

uniform sampler2D terrainTextures[numTextures];

// Terrain dynamic multi-texturing
vec4 CalcTexColor()
{
    vec4 texColor;
    float height = FragPos.y;

    if (height < heights[0]) {
       texColor = texture(terrainTextures[0], TexCoord);
    } else if (height < heights[1]) {
       vec4 t0 = texture(terrainTextures[0], TexCoord);
       vec4 t1 = texture(terrainTextures[1], TexCoord);
       float d = heights[1] - heights[0];
       float f = (height - heights[0]) / d;
       texColor = mix(t0, t1, f);
    } else if (height < heights[2]) {
       vec4 t1 = texture(terrainTextures[1], TexCoord);
       vec4 t2 = texture(terrainTextures[2], TexCoord);
       float d = heights[2] - heights[1];
       float f = (height - heights[1]) / d;
       texColor = mix(t1, t2, f);
    } else if (height < heights[3]) {
       vec4 t2 = texture(terrainTextures[2], TexCoord);
       vec4 t3 = texture(terrainTextures[3], TexCoord);
       float d = heights[3] - heights[2];
       float f = (height - heights[2]) / d;
       texColor = mix(t2, t3, f);
    } else if (height < heights[4]) {
       vec4 t3 = texture(terrainTextures[3], TexCoord);
       vec4 t4 = texture(terrainTextures[4], TexCoord);
       float d = heights[4] - heights[3];
       float f = (height - heights[3]) / d;
       texColor = mix(t3, t4, f);
    } else if (height < heights[5]) {
       vec4 t4 = texture(terrainTextures[4], TexCoord);
       vec4 t5 = texture(terrainTextures[5], TexCoord);
       float d = heights[5] - heights[4];
       float f = (height - heights[4]) / d;
       texColor = mix(t4, t5, f);
    }  else {
       texColor = texture(terrainTextures[5], TexCoord);
    }

void main()
{
    vec3 normal = normalize(Normal); 
    float diffuseFactor = dot(normal, normalize(LightPos - FragPos));
    diffuseFactor = max(diffuseFactor, 0.3f);
    float nightDarknessFactor = 0.7f;

    // Multiply the diffuse factor by the night darkness factor during the night
    diffuseFactor *= mix(nightDarknessFactor, 1.0, smoothstep(-0.5, 0.5, sin(gametime*0.09)));

    vec4 dayAmbientLight = vec4(0.12f, 0.12f, 0.12f, 0.0f);
    vec4 nightAmbientLight = vec4(0.04f, 0.04f, 0.04f, 0.0f);
    vec4 ambientLight = mix(nightAmbientLight, dayAmbientLight, smoothstep(-0.5, 0.5, sin(gametime*0.09)));

	vec4 texColor = CalcTexColor();

	FragColor = ambientLight + Color * texColor * diffuseFactor;
}
