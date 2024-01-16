#ifndef FORESTWALK_TREES_H
#define FORESTWALK_TREES_H

#include <string>
#include <vector>
#include "GL/glew.h"
#include "glm/glm.hpp"
#include "loadShaders.h"
#include "SOIL.h"
#include "../Terrain/terrain.h"

#define INSTANCE_COUNT 10

class Trees {
public:
	Trees(Terrain& terrain) : terrain(terrain) {};
	GLuint TreesId;

	void CreateTreesVBO(float terrain_width, float terrain_depth);
	void CreateTreesShader();

	void TreesInit();
	void TreesRender(glm::mat4 view, glm::mat4 projection, glm::mat4 model, float time);

	void DestroyTreesShader();
	void DestroyTreesVBO();
private:
	GLuint
		VboTrees,
		VbMatModelTrees,
		VaoTrees,
		EboTrees;

	GLint
		viewLocationTrees,
		projLocationTrees,
		modelLocationTrees;

	unsigned int cntLoadedTextures;
	std::vector<GLuint> textureIDs = { 0, 1 };

	std::vector<std::string> textures = {
			"textures/trees/tree_bark.jpg",
			"textures/trees/tree_leaves.jpg",
	};

	Terrain& terrain;

	float const PI = 3.141592f;

	// Elemente pentru reprezentarea suprafetei
	// (1) intervalele pentru parametrii considerati (u si v)
	float const U_MIN = -PI, U_MAX = PI, V_MIN = 0, V_MAX = 2 * PI;
	// (2) numarul de paralele/meridiane, de fapt numarul de valori ptr parametri
	int const NR_PARR = 20, NR_MERID = 20;
	// (3) pasul cu care vom incrementa u, respectiv v
	float step_u = (U_MAX - U_MIN) / NR_PARR, step_v = (V_MAX - V_MIN) / NR_MERID;

	const int verticesOffset = 7;
	const int indicesOffset = 18;

	struct Vertex {
		glm::vec3 Position;
		glm::vec2 TexCoords;
		glm::vec3 Normal = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 Tangent = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 Bitangent = glm::vec3(0.0f, 0.0f, 0.0f);
	};

	std::vector<Vertex> Vertices;
	std::vector<GLuint> Indices;

	unsigned int LoadTextures(std::vector<GLuint>& textureIDs, const std::vector<std::string>& textures);
};

#endif //FORESTWALK_TREES_H
