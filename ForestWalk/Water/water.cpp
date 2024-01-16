#include <vector>
#include "GL/glew.h"
#include "glm/glm.hpp"
#include "loadShaders.h"


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

void CreateWaterVBO(int width,int wordFac) {
    float y = 10.f;
    std::vector<GLfloat>Vertices;
    std::vector<GLuint>Indices;
    int dim = width*wordFac;
    int pas = wordFac;
    int now =0;
    for(int i=0;i<dim;i+=pas) {
        for (int j = 0; j < dim; j+=pas) {

            Vertices.push_back(float(i));
            Vertices.push_back(float(y));
            Vertices.push_back(float(j));

            Vertices.push_back(float(i+pas));
            Vertices.push_back(float(y));
            Vertices.push_back(float(j));

            Vertices.push_back(float(i+pas));
            Vertices.push_back(float(y));
            Vertices.push_back(float(j+pas));

            Vertices.push_back(float(i));
            Vertices.push_back(float(y));
            Vertices.push_back(float(j+pas));

            Indices.push_back(now);
            Indices.push_back(now+1);
            Indices.push_back(now+2);
            Indices.push_back(now+2);
            Indices.push_back(now+3);
            Indices.push_back(now);
            now +=4;
        }
    }
    nr_puncte=Indices.size();


    glGenVertexArrays(1, &VaoWater);
    glBindVertexArray(VaoWater);

    //  Se creeaza un buffer pentru VARFURI - COORDONATE, CULORI si TEXTURARE;
    glGenBuffers(1, &VboWater);
    glBindBuffer(GL_ARRAY_BUFFER, VboWater);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices[0])* Vertices.size(), Vertices.data(), GL_STATIC_DRAW);

    //	Se creeaza un buffer pentru INDICI;
    glGenBuffers(1, &EboWater);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboWater);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices[0])* Indices.size(), Indices.data(), GL_STATIC_DRAW);

    //	Se activeaza lucrul cu atribute;
    //  Se asociaza atributul (0 = coordonate) pentru shader;
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *) 0);

}

void CreateWaterShader() {
    WaterId = LoadShaders("shaders/water.vert", "shaders/water.frag");
}

void DestroyWaterShader() {
    glDeleteProgram(WaterId);
}

void DestroyWaterVBO() {
    glDisableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &VboWater);
    glDeleteBuffers(1, &EboWater);

    glBindVertexArray(0);
    glDeleteVertexArrays(1, &VaoWater);
}

void WaterInit() {
    glUseProgram(WaterId);

    viewLocationWater = glGetUniformLocation(WaterId, "view");
    projLocationWater = glGetUniformLocation(WaterId, "projection");
    modelLocationWater = glGetUniformLocation(WaterId, "model");
}

void WaterRender(glm::mat4 view, glm::mat4 projection, glm::mat4 model, float time) {
    glUseProgram(WaterId);

    glUniformMatrix4fv(viewLocationWater, 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(projLocationWater, 1, GL_FALSE, &projection[0][0]);
    glUniformMatrix4fv(modelLocationWater, 1, GL_FALSE, &model[0][0]);

    glBindVertexArray(VaoWater);

    glUniform1f(glGetUniformLocation(WaterId, "gametime"), time);

    glDrawElements(GL_TRIANGLES, nr_puncte, GL_UNSIGNED_INT, 0);


//    glBindTexture(GL_TEXTURE_2D, 0);
}
