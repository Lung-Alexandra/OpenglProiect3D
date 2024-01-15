#ifndef FORESTWALK_TERRAIN_H
#define FORESTWALK_TERRAIN_H

#include <string>
#include <vector>
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

	glm::vec3 lightPos = glm::vec3(0.0f, 1000.0f, 0.0f);

	float WORLD_SCALE = 4.0f;
	float TEXTURE_SCALE = 8.0f;
	float MIN_ALTITUDE = 1000.0f, MAX_ALTITUDE = -1000.0f;
	float altitude_factor = 100.0f;

	unsigned int cntLoadedTextures;
	std::vector<GLuint> textureIDs = { 0, 1, 2, 3, 4, 5 };

	std::vector<std::string> textures = {
			"textures/terrain/water.png",
			"textures/terrain/rock.jpg",
			"textures/terrain/brown_mud.jpg",
			"textures/terrain/dark_grass.png",
			"textures/terrain/grass.png",
			"textures/terrain/snow.png",
	};

	void CreateTerrainVBO(int width, int depth);
	void CreateTerrainShader();

	void TerrainInit();
	void TerrainRender(glm::mat4 view, glm::mat4 projection, glm::mat4 model, float time);

	void DestroyTerrainShader();
	void DestroyTerrainVBO();

	float GetWorldScale() const { return WORLD_SCALE; }
	void SetWorldScale(float scale) { WORLD_SCALE = scale; }

	float GetAltitudeFactor() { return altitude_factor; }
	void SetAltitudeFactor(float factor) { altitude_factor = factor; }

	glm::vec3 GetLightPos() { return lightPos; }
	void SetLightPos(glm::vec3 pos) { lightPos = pos * WORLD_SCALE; }

	float GetMinAltitude() { return MIN_ALTITUDE; }
	float GetMaxAltitude() const { return MAX_ALTITUDE; }

	glm::vec3 GetLightPos() { return lightPos; }
	void SetLightPos(glm::vec3 pos) { lightPos = pos * WORLD_SCALE; }

	float GetMinAltitude() { return MIN_ALTITUDE; }
	float GetMaxAltitude() { return MAX_ALTITUDE; }

private:
	struct Vertex {
		glm::vec3 Position;
		glm::vec2 TexCoords;
		glm::vec3 Normal = glm::vec3(0.0f, 0.0f, 0.0f);
	};

	float GetY(int x, int z, unsigned char* heightmap, int h_width, int h_height, int h_nrChannels);

	unsigned int LoadTextures(std::vector<GLuint>& textureIDs, const std::vector<std::string>& textures);
};

#endif //FORESTWALK_TERRAIN_H