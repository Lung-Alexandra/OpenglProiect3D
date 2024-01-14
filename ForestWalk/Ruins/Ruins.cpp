#include "Ruins.h"
#include "SOIL.h"
#include <string>



void Ruins::CreateRuinsShader() {
    RuinsId = LoadShaders("shaders/Ruins.vert", "shaders/Ruins.frag");

}
void Ruins::ruinsInit() {
    glUseProgram(RuinsId);
    //	Instantierea variabilelor uniforme pentru a "comunica" cu shaderele;
    viewLocation = glGetUniformLocation(RuinsId, "view");
    projLocation = glGetUniformLocation(RuinsId, "projection");
    codColLocation = glGetUniformLocation(RuinsId, "codCol");

    cntLoadedTextures = LoadTextures(textureIDs, textures);
}

unsigned int Ruins::LoadTextures(std::vector<GLuint>& textureIDs, const std::vector<std::string>& textures) {
    glGenTextures(textures.size(), textureIDs.data());

    for (GLuint i = 0; i < textures.size(); i++) {
        glBindTexture(GL_TEXTURE_2D, textureIDs[i]);

        int width, height, nrChannels;
        unsigned char* image = SOIL_load_image(textures[i].c_str(), &width, &height, &nrChannels, SOIL_LOAD_RGB);
        std::cout << width << ' ' << height << std::endl;

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

static float randomRange(float min, float max) {
    // Return a random float between min and max
    return min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min)));
}
void Ruins::CreateRuinsVBO()
{
    //	COORDONATELE varfurilor;
    GLfloat Vertices[] = {
        -50.0f, -50.0f, -50.0f, 1.0f,
         50.0f, -50.0f, -50.0f, 1.0f,
         50.0f,  50.0f, -50.0f, 1.0f,
        -50.0f,  50.0f, -50.0f, 1.0f,
        -50.0f, -50.0f,  50.0f, 1.0f,
         50.0f, -50.0f,  50.0f, 1.0f,
         50.0f,  50.0f,  50.0f, 1.0f,
        -50.0f,  50.0f,  50.0f, 1.0f
    };

    //	CULORILE instantelor;
    //	Culorile sunt generate in functie de indexul de instatiere - fiecare cub va avea o singura culoare;
    glm::vec4 Colors[INSTANCE_COUNT];
    for (int i = 0; i < INSTANCE_COUNT; i++) {
        float a = float(i) / 4.0f;
        float b = float(i) / 5.0f;
        float c = float(i) / 6.0f;
        Colors[i][0] = 0.35f + 0.30f * (sinf(a + 2.0f) + 1.0f);
        Colors[i][1] = 0.25f + 0.25f * (sinf(b + 3.0f) + 1.0f);
        Colors[i][2] = 0.25f + 0.35f * (sinf(c + 4.0f) + 1.0f);
        Colors[i][3] = 1.0f;
    }

    //  MATRICELE instantelor - se defineste tiparul de desenare al cuburilor prin ROTIREA si TRANSLATIA cubului initial (INSTANCED RENDERING);
    glm::mat4 MatModel[INSTANCE_COUNT];
    float radius = 500.0f; // Radius for the circular arrangement
    float angleDiff = 360.0f / INSTANCE_COUNT; // Angle difference between each cube

    for (int i = 0; i < INSTANCE_COUNT; i++) {
        float angle = glm::radians(i * angleDiff);
        glm::vec3 pos = glm::vec3(
            radius * cos(angle), // X position
            0,                  // Y position (ground level)
            radius * sin(angle) // Z position
        );

        float verticalScale = randomRange(5.0f, 10.0f); // Increased vertical scale for stretching
        glm::vec3 scale = glm::vec3(2.0f, verticalScale, 2.0f); // Scaling the cubes

        float rotation_angle = randomRange(-15.0f, 15.0f); // Slight random rotation
        glm::vec3 rotation_axis = glm::vec3(0, 1, 0); // Rotation around Y axis

        MatModel[i] = glm::translate(glm::mat4(1.0f), pos)
            * glm::rotate(glm::mat4(1.0f), glm::radians(rotation_angle), rotation_axis)
            * glm::scale(glm::mat4(1.0f), scale);
    }

    //  Indicii pentru varfuri;
    GLubyte Indices[] = {
        0, 2, 1, 0, 3, 2, // Bottom
        4, 5, 6, 4, 6, 7, // Top
        0, 1, 5, 0, 5, 4, // Front
        1, 2, 6, 1, 6, 5, // Right
        2, 3, 7, 2, 7, 6, // Back
        3, 0, 4, 3, 4, 7  // Left
    };

    //  Transmiterea datelor prin buffere;

    //  Se creeaza / se leaga un VAO (Vertex Array Object) - util cand se utilizeaza mai multe VBO;
    //  Generarea VAO si indexarea acestuia catre variabila VaoId;
    glGenVertexArrays(1, &VaoId);
    glBindVertexArray(VaoId);

    //  Se creeaza un buffer pentru COORDONATE;
    //  Generarea bufferului si indexarea acestuia catre variabila VbPos;
    glGenBuffers(1, &VboId);
    //  Setarea tipului de buffer - atributele varfurilor;
    glBindBuffer(GL_ARRAY_BUFFER, VboId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
    //	Se activeaza lucrul cu atribute;
    //  Se asociaza atributul (0 = coordonate) pentru shader;
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);


    GLfloat TexCoords[] = { 0.0, 0.0, 2.0, 0.0, 2.0, 5.0, 0.0, 5.0, 
    0.0, 0.0, 2.0, 0.0, 2.0, 5.0, 0.0, 5.0, 
    0.0, 0.0, 2.0, 0.0, 2.0, 5.0, 0.0, 5.0, 
    0.0, 0.0, 2.0, 0.0, 2.0, 5.0, 0.0, 5.0
    };

    // Generate and bind buffer for texture coordinates
    GLuint VbTex;
    glGenBuffers(1, &VbTex);
    glBindBuffer(GL_ARRAY_BUFFER, VbTex);
    glBufferData(GL_ARRAY_BUFFER, sizeof(TexCoords), TexCoords, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);


    glGenBuffers(1, &VbModelMat);
    glBindBuffer(GL_ARRAY_BUFFER, VbModelMat);
    glBufferData(GL_ARRAY_BUFFER, sizeof(MatModel), MatModel, GL_STATIC_DRAW);
    //	Se activeaza lucrul cu atribute pentru fiecare coloana din fiecare matrice de instantiere;
    //	Se incepe de la primul indice de atribut disponibil (2) si se creeaza pe rand cate unul pentru fiecare coloana (2+i, unde i = 0..3);
    for (int i = 0; i < 4; i++) {
        glEnableVertexAttribArray(2 + i);
        glVertexAttribPointer(2 + i,              //  Location;
            4, GL_FLOAT, GL_FALSE,                //  vec4;
            sizeof(glm::mat4),                    //  Stride;
            (void*)(sizeof(glm::vec4) * i));      //  Start offset;
        glVertexAttribDivisor(2 + i, 1);
    }

    

    //	Se creeaza un buffer pentru INDICI;
    glGenBuffers(1, &EboId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);
}

void Ruins::DestroyRuinsVBO() {
    //  Eliberarea atributelor din shadere (pozitie, culoare, texturare etc.);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);

    //  Stergerea bufferelor pentru VARFURI (Coordonate, Culori, Instanitere), INDICI;
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &VboId);
    glDeleteBuffers(1, &VbCol);
    glDeleteBuffers(1, &VbModelMat);
    glDeleteBuffers(1, &EboId);

    //  Eliberaea obiectelor de tip VAO;
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &VaoId);
}
void Ruins::render(glm::mat4 view, glm::mat4 projection, glm::mat4 model, float time)
{
    glUseProgram(RuinsId);
    glBindVertexArray(VaoId);
    glBindBuffer(GL_ARRAY_BUFFER, VboId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId);

    // Matricea de vizualizare;
    //view = camera.GetViewMatrix();
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

    //	Realizarea proiectiei;
    glUniformMatrix4fv(projLocation, 1, GL_FALSE, &projection[0][0]);

    for (GLuint i = 0; i < cntLoadedTextures; i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, textureIDs[i]);
        glUniform1i(glGetUniformLocation(RuinsId, ("ruinsTextures[" + std::to_string(i) + "]").c_str()), i);
    }

    codCol = 0;
    glUniform1i(codColLocation, codCol);
    glDrawElementsInstanced(GL_TRIANGLES, 6 * 6, GL_UNSIGNED_BYTE, 0, INSTANCE_COUNT); // 6 faces * 2 triangles each * 3 vertices

}