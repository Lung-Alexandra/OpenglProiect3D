#ifndef FORESTWALK_TERRAIN_H
#define FORESTWALK_TERRAIN_H

#include "GL/glew.h"
#include "glm/glm.hpp"
#include "loadShaders.h"
#include "SOIL.h"

class Terrain {
public:
	int t_width, t_depth;
	GLuint
			VboTerrain,
			VaoTerrain,
			EboTerrain,
			TerrainId;
	GLint
		viewLocationTerrain,
		projLocationTerrain,
		modelLocationTerrain;

	float WORLD_SCALE = 4.0f;
	float altitude_factor = 100.0f;

	void CreateTerrainVBO(int width, int depth);
	void CreateTerrainShader();
	void TerrainInit();
	void TerrainRender(glm::mat4 view, glm::mat4 projection, glm::mat4 model);

	void DestroyTerrainShader();
	void DestroyTerrainVBO();

	float get_world_scale() { return WORLD_SCALE; }
	void set_world_scale(float scale) { WORLD_SCALE = scale; }

	float get_altitude_factor() { return altitude_factor; }
	void set_altitude_factor(float factor) { altitude_factor = factor; }

private:
	struct Vertex {
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
	};

	float get_y(int x, int z, unsigned char* heightmap, int h_width, int h_height, int h_nrChannels);

	void TerrainUnbindTexture();
	void TerrainBindTexture(GLuint &textureID);
};

#endif //FORESTWALK_TERRAIN_H