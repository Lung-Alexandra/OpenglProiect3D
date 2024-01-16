#include "trees.h"
#include "SOIL.h"
#include <iostream>
#include <stdlib.h> 
#include <stdio.h>
#include <GL/freeglut.h>    
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"


unsigned int Trees::LoadTextures(std::vector<GLuint>& textureIDs, const std::vector<std::string>& textures) {
	glGenTextures(textures.size(), textureIDs.data());

	for (GLuint i = 0; i < textures.size(); i++) {
		glBindTexture(GL_TEXTURE_2D, textureIDs[i]);

		int width, height, nrChannels;
		unsigned char* image = SOIL_load_image(textures[i].c_str(), &width, &height, &nrChannels, SOIL_LOAD_RGB);

		if (image) {
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RGB,
				width, height, 0,
				GL_RGB,
				GL_UNSIGNED_BYTE,
				image
			);
			SOIL_free_image_data(image);
		}
		else {
			std::cerr << "Error loading texture: " << textures[i] << std::endl;
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);

		// Set texture wrapping options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// Set texture filtering options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		// Generate mipmaps
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	return textureIDs.size();
}

static int getRandomInt(int min, int max) {
	return min + rand() % (max - min);
}

void Trees::CreateTreesVBO(float terrain_width, float terrain_depth) {
	Vertices.resize(verticesOffset + (NR_PARR + 1) * NR_MERID);

	// base
	Vertices[0].Position = glm::vec3(20.0f, 0.0f, 0.0f);
	Vertices[0].TexCoords = glm::vec2(0.0f, 0.0f);
	Vertices[1].Position = glm::vec3(10.0f, 0.0f, 20.0f);
	Vertices[1].TexCoords = glm::vec2(1.0f, 0.0f);
	Vertices[2].Position = glm::vec3(-10.0f, 0.0f, 20.0f);
	Vertices[2].TexCoords = glm::vec2(0.0f, 0.0f);
	Vertices[3].Position = glm::vec3(-20.0f, 0.0f, 0.0f);
	Vertices[3].TexCoords = glm::vec2(1.0f, 0.0f);
	Vertices[4].Position = glm::vec3(-10.0f, 0.0f, -20.0f);
	Vertices[4].TexCoords = glm::vec2(0.0f, 0.0f);
	Vertices[5].Position = glm::vec3(10.0f, 0.0f, -20.0f);
	Vertices[5].TexCoords = glm::vec2(1.0f, 0.0f);

	// top
	Vertices[6].Position = glm::vec3(0.0f, 800.0f, 0.0f);
	Vertices[6].TexCoords = glm::vec2(0.5f, 1.0f);

	srand(time(NULL));

	glm::mat4 MatModel[INSTANCE_COUNT];
	for (int i = 0; i < INSTANCE_COUNT; i++) {
		int x_offset = terrain_width / 3.5;
		int z_offset = terrain_depth / 3.5;
		int x = getRandomInt(x_offset, (int)terrain_width - x_offset);
		int z = getRandomInt(z_offset, (int)terrain_depth - z_offset);
		float y = terrain.GetHeight(x, z);
		// std::cout << "Tree " << i << " at " << x << " " << y << " " << z << std::endl;
		glm::vec3 pos = glm::vec3(x * terrain.GetWorldScale(), y, z * terrain.GetWorldScale());
		MatModel[i] = glm::translate(glm::mat4(1.0f), pos);
	}

	Indices.resize(indicesOffset + 2 * (NR_PARR + 1) * NR_MERID + 4 * (NR_PARR + 1) * NR_MERID);

	// faces
	Indices[0] = 0;
	Indices[1] = 1;
	Indices[2] = 6;

	Indices[3] = 1;
	Indices[4] = 2;
	Indices[5] = 6;

	Indices[6] = 2;
	Indices[7] = 3;
	Indices[8] = 6;

	Indices[9] = 3;
	Indices[10] = 4;
	Indices[11] = 6;

	Indices[12] = 4;
	Indices[13] = 5;
	Indices[14] = 6;
	
	Indices[15] = 5;
	Indices[16] = 0;
	Indices[17] = 6;

	// Add each triangle normal to each of its vertices normals
	for (int i = 0; i < indicesOffset; i += 3) {
		int i0 = Indices[i];
		int i1 = Indices[i + 1];
		int i2 = Indices[i + 2];
		glm::vec3 v1 = Vertices[i1].Position - Vertices[i0].Position;
		glm::vec3 v2 = Vertices[i2].Position - Vertices[i0].Position;
		glm::vec3 normal = glm::normalize(glm::cross(v1, v2));

		Vertices[i0].Normal += normal;
		Vertices[i1].Normal += normal;
		Vertices[i2].Normal += normal;
	}

	// Normalize each vertex normal
	for (int i = 0; i < verticesOffset; i++) {
		Vertices[i].Normal = glm::normalize(Vertices[i].Normal);
	}

	// Add each triangle tangent to each of its vertices tangents
	for (int i = 0; i < indicesOffset; i += 3) {
		int i0 = Indices[i];
		int i1 = Indices[i + 1];
		int i2 = Indices[i + 2];
		glm::vec3 v1 = Vertices[i1].Position - Vertices[i0].Position;
		glm::vec3 v2 = Vertices[i2].Position - Vertices[i0].Position;
		glm::vec2 uv1 = Vertices[i1].TexCoords - Vertices[i0].TexCoords;
		glm::vec2 uv2 = Vertices[i2].TexCoords - Vertices[i0].TexCoords;

		float r = 1.0f / (uv1.x * uv2.y - uv1.y * uv2.x);
		glm::vec3 tangent = (v1 * uv2.y - v2 * uv1.y) * r;

		Vertices[i0].Tangent += tangent;
		Vertices[i1].Tangent += tangent;
		Vertices[i2].Tangent += tangent;
	}

	// Normalize each vertex tangent
	for (int i = 0; i < verticesOffset; i++) {
		Vertices[i].Tangent = glm::normalize(Vertices[i].Tangent);
	}

	// Add each triangle bitangent to each of its vertices bitangents
	for (int i = 0; i < indicesOffset; i += 3) {
		int i0 = Indices[i];
		int i1 = Indices[i + 1];
		int i2 = Indices[i + 2];
		glm::vec3 v1 = Vertices[i1].Position - Vertices[i0].Position;
		glm::vec3 v2 = Vertices[i2].Position - Vertices[i0].Position;
		glm::vec2 uv1 = Vertices[i1].TexCoords - Vertices[i0].TexCoords;
		glm::vec2 uv2 = Vertices[i2].TexCoords - Vertices[i0].TexCoords;

		float r = 1.0f / (uv1.x * uv2.y - uv1.y * uv2.x);
		glm::vec3 bitangent = (v2 * uv1.x - v1 * uv2.x) * r;

		Vertices[i0].Bitangent += bitangent;
		Vertices[i1].Bitangent += bitangent;
		Vertices[i2].Bitangent += bitangent;
	}

	// Normalize each vertex bitangent
	for (int i = 0; i < verticesOffset; i++) {
		Vertices[i].Bitangent = glm::normalize(Vertices[i].Bitangent);
	}

	// Generate leaves by implementing Rosillo curve
	float a = 500.0f, c = 1000.0f, d = 1500.0f;
	int index, index_aux;

	for (int merid = 0; merid < NR_MERID; merid++)
	{
		for (int parr = 0; parr < NR_PARR + 1; parr++)
		{
			// implementarea reprezentarii parametrice 
			float u = U_MIN + parr * step_u; // valori pentru u si v
			float v = V_MIN + merid * step_v;
			// float x_vf = cosf(v) - sinf(v) * a * sinf(u) * (c - a * cosf(u)) / (d - a * cosf(u));
			// float y_vf = sinf(v) + cosf(v) * a * sinf(u) * (c - a * cosf(u)) / (d - a * cosf(u)) + 500.0f;
			// float z_vf = a * cosf(u);
			float x_vf = v * cosf(v) + sinf(v) * a * sinf(u) * (c - a * cosf(u)) / (d - a * cosf(u));
			float y_vf = a * cosf(u) + 800.0f;
			float z_vf = -v * sinf(v) + cosf(v) * a * sinf(u) * (c - a * cosf(u)) / (d - a * cosf(u));

			// identificator ptr varf; coordonate + culoare + indice la parcurgerea meridianelor
			index = merid * (NR_PARR + 1) + parr;
			Vertices[verticesOffset + index].Position = glm::vec3(x_vf, y_vf, z_vf);
			Vertices[verticesOffset + index].TexCoords = glm::vec2(float(merid % 2), float(parr % 2));
			
			// compute normal given that the center of the egg is at (0, 800, 0)
			Vertices[verticesOffset + index].Normal = glm::normalize(glm::vec3(x_vf, -800.0f + y_vf, z_vf));

			// compute tangent given that the center of the egg is at (0, 500, 0) and (0, 1, 0) is the up vector, use cross product
			Vertices[verticesOffset + index].Tangent = glm::normalize(glm::cross(glm::vec3(0, 1, 0), glm::normalize(glm::vec3(x_vf, y_vf - 800.0f, z_vf))));

			// compute bitangent by cross product the normal and the tangent
			Vertices[verticesOffset + index].Bitangent = glm::normalize(glm::cross(Vertices[verticesOffset + index].Normal, Vertices[verticesOffset + index].Tangent));

			// ensure all is normalized
			Vertices[verticesOffset + index].Normal = glm::normalize(Vertices[verticesOffset + index].Normal);
			Vertices[verticesOffset + index].Tangent = glm::normalize(Vertices[verticesOffset + index].Tangent);
			Vertices[verticesOffset + index].Bitangent = glm::normalize(Vertices[verticesOffset + index].Bitangent);

			if (verticesOffset + index < 6) {
				std::cout << "Vertex " << verticesOffset + index << ": " << std::endl;
			}

			Indices[indicesOffset + index] = verticesOffset + index;

			// indice ptr acelasi varf la parcurgerea paralelelor
			index_aux = parr * (NR_MERID) + merid;
			Indices[indicesOffset + (NR_PARR + 1) * NR_MERID + index_aux] = verticesOffset + index;

			// indicii pentru desenarea fetelor, pentru varful curent sunt definite 4 varfuri
			if ((parr + 1) % (NR_PARR + 1) != 0) // varful considerat sa nu fie Polul Nord
			{
				int AUX = 2 * (NR_PARR + 1) * NR_MERID;
				int index1 = index; // varful v considerat
				int index2 = index + (NR_PARR + 1); // dreapta lui v, pe meridianul urmator
				int index3 = index2 + 1;  // dreapta sus fata de v
				int index4 = index + 1;  // deasupra lui v, pe acelasi meridian
				if (merid == NR_MERID - 1)  // la ultimul meridian, trebuie revenit la meridianul initial
				{
					index2 = index2 % (NR_PARR + 1);
					index3 = index3 % (NR_PARR + 1);
				}

				Indices[indicesOffset + AUX + 4 * index] = verticesOffset + index1;  // unele valori ale lui Indices, corespunzatoare Polului Nord, au valori neadecvate
				Indices[indicesOffset + AUX + 4 * index + 1] = verticesOffset + index2;
				Indices[indicesOffset + AUX + 4 * index + 2] = verticesOffset + index3;
				Indices[indicesOffset + AUX + 4 * index + 3] = verticesOffset + index4;
			}
		}
	};

	glGenVertexArrays(1, &VaoTrees);
	glBindVertexArray(VaoTrees);

	glGenBuffers(1, &VboTrees);
	glBindBuffer(GL_ARRAY_BUFFER, VboTrees);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices[0]) * Vertices.size(), Vertices.data(), GL_STATIC_DRAW);

	size_t CntFloats = 0;

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(CntFloats * sizeof(float)));
	CntFloats += 3;

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(CntFloats * sizeof(float)));
	CntFloats += 2;

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(CntFloats * sizeof(float)));
	CntFloats += 3;

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(CntFloats * sizeof(float)));
	CntFloats += 3;

	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(CntFloats * sizeof(float)));
	CntFloats += 3;

	glGenBuffers(1, &VbMatModelTrees);
	glBindBuffer(GL_ARRAY_BUFFER, VbMatModelTrees);
	glBufferData(GL_ARRAY_BUFFER, sizeof(MatModel), MatModel, GL_STATIC_DRAW);

	//	Se activeaza lucrul cu atribute pentru fiecare coloana din fiecare matrice de instantiere;
	//	Se incepe de la primul indice de atribut disponibil (5) si se creeaza pe rand cate unul pentru fiecare coloana (5 + i, unde i = 0..3);
	const int prevBuff = 5;
	for (int i = 0; i < 4; i++) {
		glEnableVertexAttribArray(prevBuff + i);
		glVertexAttribPointer(prevBuff + i,       //  Location;
			4, GL_FLOAT, GL_FALSE,                //  vec4;
			sizeof(glm::mat4),                    //  Stride;
			(void*)(sizeof(glm::vec4) * i));      //  Start offset;
		glVertexAttribDivisor(prevBuff + i, 1);
	}

	glGenBuffers(1, &EboTrees);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboTrees);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices[0]) * Indices.size(), &Indices[0], GL_STATIC_DRAW);

}

void Trees::CreateTreesShader() {
	TreesId = LoadShaders("shaders/trees.vert", "shaders/trees.frag");
}

void Trees::DestroyTreesShader() {
	glDeleteProgram(TreesId);
}

void Trees::DestroyTreesVBO() {

	// glDisableVertexAttribArray(8);
	// glDisableVertexAttribArray(7);
	// glDisableVertexAttribArray(6);
	glDisableVertexAttribArray(5);
	glDisableVertexAttribArray(4);
	glDisableVertexAttribArray(3);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &VboTrees);
	glDeleteBuffers(1, &EboTrees);
	glDeleteBuffers(1, &VbMatModelTrees);

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VaoTrees);
}


void Trees::TreesInit() {
	glUseProgram(TreesId);

	viewLocationTrees = glGetUniformLocation(TreesId, "view");
	projLocationTrees = glGetUniformLocation(TreesId, "projection");
	modelLocationTrees = glGetUniformLocation(TreesId, "model");
	codColLocationTrees = glGetUniformLocation(TreesId, "codCol");

	cntLoadedTextures = LoadTextures(textureIDs, textures);
}

void Trees::TreesRender(glm::mat4 view, glm::mat4 projection, glm::mat4 model, float time) {
	glDepthFunc(GL_LEQUAL);
	glUseProgram(TreesId);

	glUniformMatrix4fv(viewLocationTrees, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(projLocationTrees, 1, GL_FALSE, &projection[0][0]);
	glUniformMatrix4fv(modelLocationTrees, 1, GL_FALSE, &model[0][0]);

	int codCol = 0;
	glUniform1i(codColLocationTrees, codCol);

	glBindVertexArray(VaoTrees);
	glUniform1f(glGetUniformLocation(TreesId, "gametime"), time);
	glUniform3fv(glGetUniformLocation(TreesId, "LightPos"), 1, &terrain.lightPos[0]);

	for (GLuint i = 0; i < cntLoadedTextures; i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, textureIDs[i]);
		glUniform1i(glGetUniformLocation(TreesId, ("treesTextures[" + std::to_string(i) + "]").c_str()), i);
	}

	// draw trunk
	glDrawElementsInstanced(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0, INSTANCE_COUNT);

	codCol = 1;
	glUniform1i(codColLocationTrees, codCol);

	// draw leaves
	for (int patr = 0; patr < (NR_PARR + 1) * NR_MERID; patr++)
	{
		if ((patr + 1) % (NR_PARR + 1) != 0) // nu sunt considerate fetele in care in stanga jos este Polul Nord
			glDrawElementsInstanced(
				GL_QUADS,
				4,
				GL_UNSIGNED_INT,
				(GLvoid*)((indicesOffset + 2 * (NR_PARR + 1) * (NR_MERID) + 4 * patr) * sizeof(Indices[0])), INSTANCE_COUNT);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glDepthFunc(GL_LESS);
}