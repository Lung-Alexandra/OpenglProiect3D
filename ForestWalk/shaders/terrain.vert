#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 inTex;
layout (location = 2) in vec3 inNormal;

out vec4 Color;
out vec2 TexCoord;
out vec3 FragPos;
out vec3 Normal;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;
uniform float MIN_Y;
uniform float MAX_Y;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0);

	float deltaY = MAX_Y - MIN_Y;
    float ratioY = (position.y - MIN_Y) / deltaY;

	// control color lightness based on height
    float c = ratioY * 0.6 + 0.4;

    Color = vec4(c, c, c, 1.0);
	TexCoord = inTex;
	FragPos = position;
	Normal = inNormal;
}
