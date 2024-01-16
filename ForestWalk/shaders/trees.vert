#version 330

layout (location = 0) in vec4 inPosition;
layout (location = 1) in vec2 inTex;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;
layout (location = 5) in mat4 modelMatrix;

out vec2 TexCoord;
out vec3 FragPos;
out mat3 TBN; // Tangent-Bitangent-Normal Matrix
out vec3 rec_normal;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

void main(void)
{
    gl_Position = projection * view * model * modelMatrix * inPosition;
    TexCoord = inTex;
    FragPos = vec3(model * modelMatrix * inPosition); // Transform to world space

    // Compute the TBN matrix
    vec3 T = normalize(vec3(model * modelMatrix * vec4(tangent, 0.0)));
    vec3 B = normalize(vec3(model * modelMatrix * vec4(bitangent, 0.0)));
    vec3 N = normalize(vec3(model * modelMatrix * vec4(normal, 0.0)));
    TBN = mat3(T, B, N);

    rec_normal = N;
}