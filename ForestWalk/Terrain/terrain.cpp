#include "terrain.h"
#include <vector>
#include <iostream>

float Terrain::get_y(int x, int z, unsigned char* heightmap, int h_width, int h_height, int h_nrChannels) {
	// heightmap not available
	if (!heightmap) {
		return -2.0f;
	}

	// calculate heightmap array index of pixel at (x, z)
	int index = (z * h_width + x) * h_nrChannels;

	// check if index is valid (RGBA value, 4 channels expected)
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
	std::vector<Vertex> Vertices;
	Vertices.resize(t_width * t_depth);

	int idx = 0;

	for (int z = 0; z < t_depth; z++) {
		for (int x = 0; x < t_width; x++) {
			assert(idx < Vertices.size());
			int y = get_y(x, z, h_image, h_width, h_height, h_nrChannels);

			Vertices[idx].Position = glm::vec3(x * WORLD_SCALE, y * WORLD_SCALE, z * WORLD_SCALE);
			Vertices[idx].Normal = glm::vec3(0, 1, 0);
			Vertices[idx].TexCoords = glm::vec2(x, z);
			++idx;
		}
	}

	SOIL_free_image_data(h_image);

	// Initialize indices
	std::vector<GLuint> Indices;
	Indices.resize((t_width - 1) * (t_depth - 1) * 6);

	idx = 0;

	for (int z = 0; z < t_depth - 1; z++) {
		for (int x = 0; x < t_width - 1; x++) {
			unsigned int bottom_left = z * t_width + x;
			unsigned int bottom_right = bottom_left + 1;
			unsigned int top_left = (z + 1) * t_width + x;
			unsigned int top_right = top_left + 1;

			// add top left triangle
			Indices[idx++] = bottom_left;
			Indices[idx++] = top_left;
			Indices[idx++] = top_right;

			// add bottom right triangle
			Indices[idx++] = bottom_left;
			Indices[idx++] = top_right;
			Indices[idx++] = bottom_right;

		}
	}

	glGenVertexArrays(1, &VaoTerrain);
	glBindVertexArray(VaoTerrain);

	glGenBuffers(1, &VboTerrain);
	glBindBuffer(GL_ARRAY_BUFFER, VboTerrain);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices[0]) * Vertices.size(), &Vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &EboTerrain);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboTerrain);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices[0]) * Indices.size(), &Indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)0);
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

void Terrain::TerrainInit() {
	glUseProgram(TerrainId);

	viewLocationTerrain = glGetUniformLocation(TerrainId, "view");
	projLocationTerrain = glGetUniformLocation(TerrainId, "projection");
	modelLocationTerrain = glGetUniformLocation(TerrainId, "model");
}

void Terrain::TerrainRender(glm::mat4 view, glm::mat4 projection, glm::mat4 model) {
	glUseProgram(TerrainId);

	glUniformMatrix4fv(viewLocationTerrain, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(projLocationTerrain, 1, GL_FALSE, &projection[0][0]);
	glUniformMatrix4fv(modelLocationTerrain, 1, GL_FALSE, &model[0][0]);

	glBindVertexArray(VaoTerrain);

	// glDrawArrays(GL_POINTS, 0, t_width * t_depth);
	glDrawElements(GL_TRIANGLES, (t_width - 1) * (t_depth - 1) * 6, GL_UNSIGNED_INT, NULL);
}