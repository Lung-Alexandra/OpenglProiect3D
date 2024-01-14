#include "Ruins.h"


void Ruins::CreateRuinsShader() {
    RuinsId = LoadShaders("shaders/example.vert", "shaders/example.frag");

}
void Ruins::ruinsInit() {
    glUseProgram(RuinsId);
    //	Instantierea variabilelor uniforme pentru a "comunica" cu shaderele;
    viewLocation = glGetUniformLocation(RuinsId, "view");
    projLocation = glGetUniformLocation(RuinsId, "projection");
    codColLocation = glGetUniformLocation(RuinsId, "codCol");
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

    //  Se creeaza un buffer pentru CULORI;
    glGenBuffers(1, &VbCol);
    glBindBuffer(GL_ARRAY_BUFFER, VbCol);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Colors), Colors, GL_STATIC_DRAW);
    //	Se activeaza lucrul cu atribute;
    //  Se asociaza atributul (1 = culoare) pentru shader;
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (GLvoid*)0);
    //	Rata cu care are loc distribuirea culorilor per instanta;
    glVertexAttribDivisor(1, 1);

    //  Se creeaza un buffer pentru MATRICEA DE POZITIE (instantierea cuburilor);
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

    codCol = 0;
    glUniform1i(codColLocation, codCol);
    glDrawElementsInstanced(GL_TRIANGLES, 6 * 6, GL_UNSIGNED_BYTE, 0, INSTANCE_COUNT); // 6 faces * 2 triangles each * 3 vertices

}