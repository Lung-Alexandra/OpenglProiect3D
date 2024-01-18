#ifndef FORESTWALK_WATER_H
#define FORESTWALK_WATER_H

#include <vector>
#include "GL/glew.h"
#include "glm/glm.hpp"
#include "loadShaders.h"

class Water {
public:

    GLuint
            VboWater,
            VaoWater,
            EboWater,
            WaterId;
    GLint
            viewLocationWater,
            projLocationWater,
            modelLocationWater;
    int nr_puncte;

    void CreateWaterVBO(int width,int wordFac);

    void CreateWaterShader();

    void DestroyWaterShader();

    void DestroyWaterVBO();

    void WaterInit();

    void WaterRender(glm::mat4 view, glm::mat4 projection, glm::mat4 model, float time, glm::vec3 camera);



};



#endif //FORESTWALK_WATER_H
