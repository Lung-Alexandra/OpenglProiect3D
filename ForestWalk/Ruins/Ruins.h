#pragma once

// Libraries
#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "loadShaders.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "../Terrain/terrain.h"
#define INSTANCE_COUNT 10

class Ruins {
    float const PI = 3.141592;
private:
    const Terrain& terrain;

    GLuint VboId, VaoId, VbCol, VbModelMat, EboId;
    GLint viewLocation, projLocation, codColLocation,modelLocation;
    GLint codCol;
    unsigned int cntLoadedTextures;

    std::vector<GLuint> textureIDs = {0,1};

    std::vector<std::string> textures = {
            "textures/ruins/granite-wall1-tex.png",
            "textures/ruins/granite-wall1-normal.png",
    };

    unsigned int LoadTextures(std::vector<GLuint>& textureIDs, const std::vector<std::string>& textures);

public:
    Ruins(const Terrain& terrain) : terrain(terrain) {};
    GLint RuinsId;
    void CreateRuinsShader();
    void ruinsInit();
    void CreateRuinsVBO();
    void DestroyRuinsVBO();
    void render(glm::mat4 view, glm::mat4 projection, float time);
};


