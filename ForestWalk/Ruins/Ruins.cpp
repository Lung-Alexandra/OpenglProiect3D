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
    modelLocation = glGetUniformLocation(RuinsId, "model");

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
    GLfloat size = 50.0f;

    GLfloat Vertices[] = {
        // Bottom face
        -size, -size, -size, 1.0f,  // Vertex 0
         size, -size, -size, 1.0f,  // Vertex 1
         size, -size,  size, 1.0f,  // Vertex 2
        -size, -size,  size, 1.0f,  // Vertex 3

        // Top face
        -size,  size, -size, 1.0f,  // Vertex 4
         size,  size, -size, 1.0f,  // Vertex 5
         size,  size,  size, 1.0f,  // Vertex 6
        -size,  size,  size, 1.0f,  // Vertex 7

        // Front face
        -size, -size,  size, 1.0f,  // Vertex 8
         size, -size,  size, 1.0f,  // Vertex 9
         size,  size,  size, 1.0f,  // Vertex 10
        -size,  size,  size, 1.0f,  // Vertex 11

        // Right face
         size, -size, -size, 1.0f,  // Vertex 12
         size,  size, -size, 1.0f,  // Vertex 13
         size,  size,  size, 1.0f,  // Vertex 14
         size, -size,  size, 1.0f,  // Vertex 15

         // Back face
         -size, -size, -size, 1.0f,  // Vertex 16
         -size,  size, -size, 1.0f,  // Vertex 17
          size,  size, -size, 1.0f,  // Vertex 18
          size, -size, -size, 1.0f,  // Vertex 19

          // Left face
          -size, -size, -size, 1.0f,  // Vertex 20
          -size, -size,  size, 1.0f,  // Vertex 21
          -size,  size,  size, 1.0f,  // Vertex 22
          -size,  size, -size, 1.0f   // Vertex 23
    };

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

        float verticalScale = randomRange(5.0f, 10.0f) * 2; // Increased vertical scale for stretching
        glm::vec3 scale = glm::vec3(2.0f, verticalScale, 2.0f); // Scaling the cubes

        float rotation_angle = randomRange(-15.0f, 15.0f); // Slight random rotation
        glm::vec3 rotation_axis = glm::vec3(0, 1, 0); // Rotation around Y axis

        MatModel[i] = glm::translate(glm::mat4(1.0f), pos)
            * glm::rotate(glm::mat4(1.0f), glm::radians(rotation_angle), rotation_axis)
            * glm::scale(glm::mat4(1.0f), scale);
    }

    //  Indicii pentru varfuri;
    GLubyte Indices[] = {
         0, 1, 2, 3,     // Bottom face
         4, 5, 6, 7,     // Top face
         8, 9, 10, 11,   // Front face
         12, 13, 14, 15, // Right face
         16, 17, 18, 19, // Back face
         20, 21, 22, 23 // Left face
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

    GLfloat TexCoords[] = {
        // Bottom face 
        0.0, 0.0,  1.0, 0.0,  1.0, 1.0,  0.0, 1.0,

        // Top face 
        1.0, 1.0,  0.0, 1.0,  0.0, 0.0,  1.0, 0.0,

        // Front face 
       1.0f, 4.0f,  0.0f, 4.0f,  0.0f, 0.0f,  1.0f, 0.0f,

       // Right face 
       1.0f, 0.0f,  1.0f, 4.0f,  0.0f, 4.0f,  0.0f, 0.0f,

       // Back face 
       1.0f, 0.0f,  1.0f, 4.0f,  0.0f, 4.0f,  0.0f, 0.0f,

       // Left face
       1.0f, 4.0f,  0.0f, 4.0f,  0.0f, 0.0f,  1.0f, 0.0f
    };


    // Generate and bind buffer for texture coordinates
    GLuint VbTex;
    glGenBuffers(1, &VbTex);
    glBindBuffer(GL_ARRAY_BUFFER, VbTex);
    glBufferData(GL_ARRAY_BUFFER, sizeof(TexCoords), TexCoords, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);

   


    GLfloat Normals[] = {
         -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         -1.0f, -1.0f, -1.0f,
         -1.0f, 1.0f, -1.0f,
         1.0f, 1.0f, -1.0f,
         1.0f, 1.0f, -1.0f,
         -1.0f, 1.0f, -1.0f,
         -1.0f, -1.0f, 1.0f,
         1.0f, -1.0f, 1.0f,
         1.0f, -1.0f, 1.0f,
         -1.0f, -1.0f, 1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, 1.0f, -1.0f,
         1.0f, 1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         -1.0f, -1.0f, -1.0f,
         -1.0f, 1.0f, -1.0f,
         -1.0f, 1.0f, -1.0f,
         -1.0f, -1.0f, -1.0f,
         -1.0f, -1.0f, 1.0f,
         -1.0f, -1.0f, 1.0f,
         -1.0f, 1.0f, 1.0f,
         -1.0f, 1.0f, 1.0f
    };

    GLfloat Tangents[] = {
        // Bottom Face (Vertices 0, 1, 2, 3)
        1.0f, 0.0f, 0.0f, // Vertex 0
        1.0f, 0.0f, 0.0f, // Vertex 1
        1.0f, 0.0f, 0.0f, // Vertex 2
        1.0f, 0.0f, 0.0f, // Vertex 3

        // Top Face (Vertices 4, 5, 6, 7)
        1.0f, 0.0f, 0.0f, // Vertex 4
        1.0f, 0.0f, 0.0f, // Vertex 5
        1.0f, 0.0f, 0.0f, // Vertex 6
        1.0f, 0.0f, 0.0f, // Vertex 7

        // Front Face (Vertices 8, 9, 10, 11)
        1.0f, 0.0f, 0.0f, // Vertex 8
        1.0f, 0.0f, 0.0f, // Vertex 9
        1.0f, 0.0f, 0.0f, // Vertex 10
        1.0f, 0.0f, 0.0f, // Vertex 11

        // Right Face (Vertices 12, 13, 14, 15)
        0.0f, 0.0f, -1.0f, // Vertex 12
        0.0f, 0.0f, -1.0f, // Vertex 13
        0.0f, 0.0f, -1.0f, // Vertex 14
        0.0f, 0.0f, -1.0f, // Vertex 15

        // Back Face (Vertices 16, 17, 18, 19)
        -1.0f, 0.0f, 0.0f, // Vertex 16
        -1.0f, 0.0f, 0.0f, // Vertex 17
        -1.0f, 0.0f, 0.0f, // Vertex 18
        -1.0f, 0.0f, 0.0f, // Vertex 19

        // Left Face (Vertices 20, 21, 22, 23)
        0.0f, 0.0f, 1.0f, // Vertex 20
        0.0f, 0.0f, 1.0f, // Vertex 21
        0.0f, 0.0f, 1.0f, // Vertex 22
        0.0f, 0.0f, 1.0f, // Vertex 23
    };

    GLfloat Bitangents[] = {
        // Bottom Face (Vertices 0, 1, 2, 3)
        0.0f, 0.0f, 1.0f, // Vertex 0
        0.0f, 0.0f, 1.0f, // Vertex 1
        0.0f, 0.0f, 1.0f, // Vertex 2
        0.0f, 0.0f, 1.0f, // Vertex 3

        // Top Face (Vertices 4, 5, 6, 7)
        0.0f, 0.0f, -1.0f, // Vertex 4
        0.0f, 0.0f, -1.0f, // Vertex 5
        0.0f, 0.0f, -1.0f, // Vertex 6
        0.0f, 0.0f, -1.0f, // Vertex 7

        // Front Face (Vertices 8, 9, 10, 11)
        0.0f, 1.0f, 0.0f, // Vertex 8
        0.0f, 1.0f, 0.0f, // Vertex 9
        0.0f, 1.0f, 0.0f, // Vertex 10
        0.0f, 1.0f, 0.0f, // Vertex 11

        // Right Face (Vertices 12, 13, 14, 

        0.0f, 1.0f, 0.0f, // Vertex 12
        0.0f, 1.0f, 0.0f, // Vertex 13
        0.0f, 1.0f, 0.0f, // Vertex 14
        0.0f, 1.0f, 0.0f, // Vertex 15

        // Back Face (Vertices 16, 17, 18, 19)
        0.0f, 1.0f, 0.0f, // Vertex 16
        0.0f, 1.0f, 0.0f, // Vertex 17
        0.0f, 1.0f, 0.0f, // Vertex 18
        0.0f, 1.0f, 0.0f, // Vertex 19

        // Left Face (Vertices 20, 21, 22, 23)
        0.0f, 1.0f, 0.0f, // Vertex 20
        0.0f, 1.0f, 0.0f, // Vertex 21
        0.0f, 1.0f, 0.0f, // Vertex 22
        0.0f, 1.0f, 0.0f // Vertex 23
    };

    GLuint VbNormals;
    glGenBuffers(1, &VbNormals);
    glBindBuffer(GL_ARRAY_BUFFER, VbNormals);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Normals), Normals, GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

    

    GLuint VbTangents;
    glGenBuffers(1, &VbTangents);
    glBindBuffer(GL_ARRAY_BUFFER, VbTangents);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Tangents), Tangents, GL_STATIC_DRAW);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

    

    GLuint VbBiTangents;
    glGenBuffers(1, &VbBiTangents);
    glBindBuffer(GL_ARRAY_BUFFER, VbBiTangents);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Bitangents), Bitangents, GL_STATIC_DRAW);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);


    glGenBuffers(1, &VbModelMat);
    glBindBuffer(GL_ARRAY_BUFFER, VbModelMat);
    glBufferData(GL_ARRAY_BUFFER, sizeof(MatModel), MatModel, GL_STATIC_DRAW);

    //	Se activeaza lucrul cu atribute pentru fiecare coloana din fiecare matrice de instantiere;
    //	Se incepe de la primul indice de atribut disponibil (2) si se creeaza pe rand cate unul pentru fiecare coloana (2+i, unde i = 0..3);
    const int prevBuff = 5;
    for (int i = 0; i < 4; i++) {
        glEnableVertexAttribArray(prevBuff + i);
        glVertexAttribPointer(prevBuff + i,              //  Location;
            4, GL_FLOAT, GL_FALSE,                //  vec4;
            sizeof(glm::mat4),                    //  Stride;
            (void*)(sizeof(glm::vec4) * i));      //  Start offset;
        glVertexAttribDivisor(prevBuff + i, 1);
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
static float terrain_width = 1023, terrain_depth = 1023;

void Ruins::render(glm::mat4 view, glm::mat4 projection, float time)
{
    glm::mat4 model;
    glm::vec3 relModel = terrain.GetWorldScale() * glm::vec3(0,10,-250) ;
    model = glm::translate(glm::mat4(1.0f), relModel + glm::vec3(terrain.GetWorldScale() * terrain_width / 2,
        0,
        terrain.GetWorldScale() * terrain_depth / 2));


    glUseProgram(RuinsId);
    glBindVertexArray(VaoId);
    glBindBuffer(GL_ARRAY_BUFFER, VboId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId);

    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

    //	Realizarea proiectiei;
    glUniformMatrix4fv(projLocation, 1, GL_FALSE, &projection[0][0]);
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, &model[0][0]);


    glUniform1f(glGetUniformLocation(RuinsId, "gametime"), time);
    glUniform3fv(glGetUniformLocation(RuinsId, "LightPos"), 1, &terrain.lightPos[0]);

    for (GLuint i = 0; i < cntLoadedTextures; i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, textureIDs[i]);
        glUniform1i(glGetUniformLocation(RuinsId, ("ruinsTextures[" + std::to_string(i) + "]").c_str()), i);
    }
    codCol = 0;
    glUniform1i(codColLocation, codCol);
    glDrawElementsInstanced(GL_QUADS, 24, GL_UNSIGNED_BYTE, 0, INSTANCE_COUNT);

}