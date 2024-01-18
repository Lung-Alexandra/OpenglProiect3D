#include <iostream>
#include "skybox.h"


void Skybox::CreateSkyboxVBO() {
    GLfloat Vertices[] = {
            //   Coordinates
            -Size, -Size, Size,//        7--------6
            Size, -Size, Size,//        /|       /|
            Size, -Size, -Size,//      4--------5 |
            -Size, -Size, -Size,//   | |      | |
            -Size, Size, Size,//    | 3------|-2
            Size, Size, Size,//     |/       |/
            Size, Size, -Size,//    0--------1
            -Size, Size, -Size
    };

    //	Indicii care determina ordinea de parcurgere a varfurilor;
    GLuint Indices[] = {
            // Right
            1, 2, 6,
            6, 5, 1,
            // Left
            0, 4, 7,
            7, 3, 0,
            // Top
            4, 5, 6,
            6, 7, 4,
            // Bottom
            0, 3, 2,
            2, 1, 0,
            // Back
            0, 1, 5,
            5, 4, 0,
            // Front
            3, 7, 6,
            6, 2, 3
    };

    glGenVertexArrays(1, &VaoSky);
    glBindVertexArray(VaoSky);

    //  Se creeaza un buffer pentru VARFURI - COORDONATE, CULORI si TEXTURARE;
    glGenBuffers(1, &VboSky);
    glBindBuffer(GL_ARRAY_BUFFER, VboSky);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

    //	Se creeaza un buffer pentru INDICI;
    glGenBuffers(1, &EboSky);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboSky);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

    //	Se activeaza lucrul cu atribute;
    //  Se asociaza atributul (0 = coordonate) pentru shader;
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *) 0);

}

void Skybox::CreateSkyShader() {
    SkyboxId = LoadShaders("shaders/skybox.vert", "shaders/skybox.frag");
}

void Skybox::DestroySkyShader() {
    glDeleteProgram(SkyboxId);
}

void Skybox::DestroySkyboxVBO() {
    //  Eliberarea atributelor din shadere (pozitie, culoare, texturare etc.);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);

    //  Stergerea bufferelor pentru VARFURI (Coordonate, Culori, Textura), INDICI;
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &VboSky);
    glDeleteBuffers(1, &EboSky);

    //  Eliberaea obiectelor de tip VAO;
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &VaoSky);
}

unsigned int Skybox::LoadCubemap(GLuint &textureID, std::vector<std::string> faces) {
    glGenTextures(1, &textureID);
    SkyBindTexture(textureID);
    int width, height, nrChanel;
    unsigned char *image;

    for (GLuint i = 0; i < faces.size(); i++) {
        image = SOIL_load_image(faces[i].c_str(), &width, &height, &nrChanel, SOIL_LOAD_RGBA);
        glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
                GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image
        );
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return textureID;
}

void Skybox::SkyInit() {
    glUseProgram(SkyboxId);
    //	Instantierea variabilelor uniforme pentru a "comunica" cu shaderele;
    viewLocationSky = glGetUniformLocation(SkyboxId, "view");
    projLocationSky = glGetUniformLocation(SkyboxId, "projection");
    rotLocationSky = glGetUniformLocation(SkyboxId, "rotation");

    cubemapTextureDay = LoadCubemap(textureDay,day);
    cubemapTextureNight = LoadCubemap(textureNight,night);
}

void Skybox::SkyRender(glm::mat4 view, glm::mat4 projection, float time) {

    glDepthFunc(GL_LEQUAL);
    glUseProgram(SkyboxId);

    // Transmiterea matricei de vizualizare;
    glUniformMatrix4fv(viewLocationSky, 1, GL_FALSE, &view[0][0]);

    // Transmiterea matricei de proiectiei;
    glUniformMatrix4fv(projLocationSky, 1, GL_FALSE, &projection[0][0]);
    float rotate = Speed * time;
    glUniform1f(glGetUniformLocation(SkyboxId, "angle"), rotate);
    glUniform1f(glGetUniformLocation(SkyboxId, "gametime"), time);

    glBindVertexArray(VaoSky);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTextureDay);
    glUniform1i(glGetUniformLocation(SkyboxId, "skyboxday"), 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTextureNight);
    glUniform1i(glGetUniformLocation(SkyboxId, "skyboxnight"), 1);

    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glDepthFunc(GL_LESS);
//    SkyUnbindTexture();
}

void Skybox::SkyUnbindTexture() {
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void Skybox::SkyBindTexture(GLuint &textureID){
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
}