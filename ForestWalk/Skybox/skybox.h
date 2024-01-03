#ifndef FORESTWALK_SKYBOX_H
#define FORESTWALK_SKYBOX_H


#include <string>
#include <vector>
#include "GL/glew.h"
#include "glm/glm.hpp"
#include "loadShaders.h"
#include "SOIL.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
class Skybox {
public:
    GLuint
            VboSky,
            VaoSky,
            EboSky,
            SkyboxId,
            textureDay,
            textureNight;
    GLint   viewLocationSky,
            projLocationSky,
            rotLocationSky;
    float Speed = 0.02f;
    float Size = 1;
    unsigned int cubemapTextureDay, cubemapTextureNight;
    std::vector<std::string> day = {
            "textures/skybox/dayRight.png",
            "textures/skybox/dayLeft.png",
            "textures/skybox/dayTop.png",
            "textures/skybox/dayBottom.png",
            "textures/skybox/dayFront.png",
            "textures/skybox/dayBack.png"
    };
    std::vector<std::string> night = {
            "textures/skybox/nightRight.png",
            "textures/skybox/nightLeft.png",
            "textures/skybox/nightTop.png",
            "textures/skybox/nightBottom.png",
            "textures/skybox/nightFront.png",
            "textures/skybox/nightBack.png"
    };
    void CreateSkyboxVBO();
    void CreateSkyShader();
    void SkyInit();
    void SkyRender(glm::mat4 view,glm::mat4 projection, float time);

    void DestroySkyShader();
    void DestroySkyboxVBO();

private:
    unsigned int LoadCubemap(GLuint &textureID, std::vector<std::string> faces);
    void SkyUnbindTexture();
    void SkyBindTexture(GLuint &textureID);
};


#endif //FORESTWALK_SKYBOX_H
