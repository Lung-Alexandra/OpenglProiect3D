#include "terrain.h"
#include <vector>
#include <iostream>

float Terrain::GetY(int x, int z, unsigned char* heightmap, int h_width, int h_height, int h_nrChannels) {
	// Heightmap not available
	if (!heightmap) {
		return -2.0f;
	}

	// Calculate heightmap array index of pixel at (x, z)
	int index = (z * h_width + x) * h_nrChannels;

	// Check if index is valid (RGBA value, 4 channels expected)
	if (index + 3 >= h_width * h_height * h_nrChannels || index < 0) {
		std::cerr << "Error: Invalid index value at x=" << x << ", z=" << z << '\n';
		std::cerr << "Computed index: " << index << '\n';
		std::cerr << "Image Width: " << h_width << ", Height: " << h_height << ", Channels: " << h_nrChannels << '\n';
		SOIL_free_image_data(heightmap);
		return -2.0f;
	}

	if (x >= h_width || z >= h_height) {
		std::cerr << "Error: Invalid x or z value at x=" << x << ", z=" << z << '\n';
		std::cerr << "Image Width: " << h_width << ", Height: " << h_height << '\n';
		SOIL_free_image_data(heightmap);
		return -2.0f;
	}

	// Get RGB values of pixel at (x, z)
	unsigned char red = heightmap[index];
	unsigned char green = heightmap[index + 1];
	unsigned char blue = heightmap[index + 2];

	// Convert RGB to grayscale (assuming equal weights for red, green, and blue)
	float altitude = (red + green + blue) / (3.0f * 255.0f);

	return altitude * altitude_factor;

}

void Terrain::CreateTerrainVBO(int width, int depth) {
	t_width = width;
	t_depth = depth;

	// Load heightmap
	int h_width = 0, h_height = 0, h_nrChannels = 0;
	unsigned char* h_image = SOIL_load_image("textures/terrain/heightmap_greyscale.png", &h_width, &h_height, &h_nrChannels, SOIL_LOAD_AUTO);

	if (!h_image) {
		std::cerr << "Error loading heightmap image\n";
		SOIL_free_image_data(h_image);
	}

	// Initialize vertices
	Vertices.resize(t_width * t_depth);

	int idx = 0;

	for (int z = 0; z < t_depth; z++) {
		for (int x = 0; x < t_width; x++) {
			assert(idx < Vertices.size());
			int y = GetY(x, z, h_image, h_width, h_height, h_nrChannels);
			if (y > MAX_ALTITUDE) {
				MAX_ALTITUDE = y;
			}
			if (y < MIN_ALTITUDE) {
				MIN_ALTITUDE = y;
			}

			Vertices[idx].Position = glm::vec3(x * WORLD_SCALE, y * WORLD_SCALE, z * WORLD_SCALE);
			Vertices[idx].TexCoords = glm::vec2(TEXTURE_SCALE * x / t_width, TEXTURE_SCALE * z / t_depth);
			++idx;
		}
	}

	SOIL_free_image_data(h_image);

	// Initialize indices
	Indices.resize((t_width - 1) * (t_depth - 1) * 6);

	idx = 0;

	for (int z = 0; z < t_depth - 1; z++) {
		for (int x = 0; x < t_width - 1; x++) {
			unsigned int bottom_left = z * t_width + x;
			unsigned int bottom_right = bottom_left + 1;
			unsigned int top_left = (z + 1) * t_width + x;
			unsigned int top_right = top_left + 1;

			// Top left triangle
			Indices[idx++] = bottom_left;
			Indices[idx++] = top_left;
			Indices[idx++] = top_right;

			// Bottom right triangle
			Indices[idx++] = bottom_left;
			Indices[idx++] = top_right;
			Indices[idx++] = bottom_right;

		}
	}

	// Calculate normals

	// Add each triangle normal to each of its vertices normals
	for (int i = 0; i < Indices.size(); i += 3) {
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
	for (int i = 0; i < Vertices.size(); i++) {
		Vertices[i].Normal = glm::normalize(Vertices[i].Normal);
	}


	glGenVertexArrays(1, &VaoTerrain);
	glBindVertexArray(VaoTerrain);

	glGenBuffers(1, &VboTerrain);
	glBindBuffer(GL_ARRAY_BUFFER, VboTerrain);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices[0]) * Vertices.size(), &Vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &EboTerrain);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboTerrain);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices[0]) * Indices.size(), &Indices[0], GL_STATIC_DRAW);

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
}

void Terrain::CreateTerrainShader() {
	TerrainId = LoadShaders("shaders/terrain.vert", "shaders/terrain.frag");
}

void Terrain::DestroyTerrainShader() {
	glDeleteProgram(TerrainId);
}

void Terrain::DestroyTerrainVBO() {
	glDisableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &VboTerrain);
	glDeleteBuffers(1, &EboTerrain);

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VaoTerrain);
}

unsigned int Terrain::LoadTextures(std::vector<GLuint>& textureIDs, const std::vector<std::string>& textures) {
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

void Terrain::TerrainInit() {
	glUseProgram(TerrainId);

	viewLocationTerrain = glGetUniformLocation(TerrainId, "view");
	projLocationTerrain = glGetUniformLocation(TerrainId, "projection");
	modelLocationTerrain = glGetUniformLocation(TerrainId, "model");

	cntLoadedTextures = LoadTextures(textureIDs, textures);
}

void Terrain::TerrainRender(glm::mat4 view, glm::mat4 projection, glm::mat4 model, float time) {
	glDepthFunc(GL_LEQUAL);
	glUseProgram(TerrainId);

	glUniformMatrix4fv(viewLocationTerrain, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(projLocationTerrain, 1, GL_FALSE, &projection[0][0]);
	glUniformMatrix4fv(modelLocationTerrain, 1, GL_FALSE, &model[0][0]);

	glBindVertexArray(VaoTerrain);

	glUniform1f(glGetUniformLocation(TerrainId, "gametime"), time);
	glUniform1f(glGetUniformLocation(TerrainId, "MIN_Y"), MIN_ALTITUDE * WORLD_SCALE);
	glUniform1f(glGetUniformLocation(TerrainId, "MAX_Y"), MAX_ALTITUDE * WORLD_SCALE);
	glUniform3fv(glGetUniformLocation(TerrainId, "LightPos"), 1, &lightPos[0]);

	for (GLuint i = 0; i < cntLoadedTextures; i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, textureIDs[i]);
		glUniform1i(glGetUniformLocation(TerrainId, ("terrainTextures[" + std::to_string(i) + "]").c_str()), i);
	}

	glDrawElements(GL_TRIANGLES, (t_width - 1) * (t_depth - 1) * 6, GL_UNSIGNED_INT, NULL);

	glBindTexture(GL_TEXTURE_2D, 0);
	glDepthFunc(GL_LESS);
}