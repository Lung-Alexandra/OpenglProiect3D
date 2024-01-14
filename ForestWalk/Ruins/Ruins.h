#pragma once

// Libraries
#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "loadShaders.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#define INSTANCE_COUNT 10

class Ruins {
    float const PI = 3.141592;
private:
    GLuint VboId, VaoId, VbCol, VbModelMat, EboId;
    GLint viewLocation, projLocation, codColLocation;
    GLint codCol;
    unsigned int cntLoadedTextures;

    std::vector<GLuint> textureIDs = {0};

    std::vector<std::string> textures = {
            "textures/ruins/stone-granite-2-TEX.png",
    };

    unsigned int LoadTextures(std::vector<GLuint>& textureIDs, const std::vector<std::string>& textures);

public:
    GLint RuinsId;
    void CreateRuinsShader();
    void ruinsInit();
    void CreateRuinsVBO();
    void DestroyRuinsVBO();
    void render(glm::mat4 view, glm::mat4 projection, glm::mat4 model, float time);
};


