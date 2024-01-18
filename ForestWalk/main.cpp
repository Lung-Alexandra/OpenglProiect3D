//	Biblioteci
#include <iostream>
#include <GL/glew.h>        //  Definește prototipurile functiilor OpenGL si constantele necesare pentru programarea OpenGL moderna; 
#include <GL/freeglut.h>    //	Include functii pentru: 
//	- gestionarea ferestrelor si evenimentelor de tastatura si mouse,
//  - desenarea de primitive grafice precum dreptunghiuri, cercuri sau linii,
//  - crearea de meniuri si submeniuri;
#include "loadShaders.h"    //	Fisierul care face legatura intre program si shadere;
// 	Fisierul care face legatura intre program si shadere;
#include "glm/glm.hpp"        //	Bibloteci utilizate pentru transformari grafice;
#include "glm/gtc/matrix_transform.hpp"

#include "Camera/camera.h"
#include "Skybox/skybox.h"
#include "Terrain/terrain.h"
#include "Trees/trees.h"

//	Numarul de multiplicari 
#define INSTANCE_COUNT 10

//  Identificatorii obiectelor de tip OpenGL; 
GLuint
        VboId,
        VaoId,
        VbCol,
        VbModelMat,
        EboId;
GLint   viewLocation,
        projLocation,
        ProgramId,
        codColLocation;
GLint winWidth = 1200, winHeight = 900;
//	Variabile catre matricile de transformare;
glm::mat4 view, projection, model;
//	Variabila ce determina schimbarea culorii pixelilor in shader;
GLint codCol;
//	Valaorea lui pi;
float const PI = 3.141592;
bool keys[256]; // 100->    GLUT_KEY_LEFT, 101-> GLUT_KEY_RIGHT, 102->GLUT_KEY_UP, 103->GLUT_KEY_DOWN
//	Elemente pentru matricea de proiectie;
float width = 800, height = 600, zNear = 0.3f, fov = 90.f * PI / 180;
float gametime = 0, delta_t = 0, last_time = 0;
float terrain_width = 1023, terrain_depth = 1023;
Camera camera;
Skybox sky;
Terrain terrain;
Trees trees(terrain);

void ProcessNormalKeysUp(unsigned char key, int x, int y) {
    keys[key] = false;
}
void ProcessSpecialKeysUp(int key, int xx, int yy) {
    keys[key] = false;
}

void ProcessNormalKeysDown(unsigned char key, int x, int y) {
    keys[key] = true;
}

void ProcessNormalKeys(unsigned char key, int x, int y) {
    ProcessNormalKeysDown(key, x, y);
    if (keys['w']) {
        if (keys['d']) {
            camera.ProcessKeyboard(RFORWARD, gametime);
        } else if (keys['a']) {
            camera.ProcessKeyboard(LFORWARD, gametime);
        } else {
            camera.ProcessKeyboard(FORWARD, gametime);
        }
    }
    if (keys['s']) {
        if (keys['d']) {
            camera.ProcessKeyboard(RBACKWARD, gametime);
        } else if (keys['a']) {
            camera.ProcessKeyboard(LBACKWARD, gametime);
        } else {
            camera.ProcessKeyboard(BACKWARD, gametime);
        }
    }
    if (keys['a']) {
        if (keys['w']) {
            camera.ProcessKeyboard(LFORWARD, gametime);
        } else if (keys['s']) {
            camera.ProcessKeyboard(LBACKWARD, gametime);
        } else {
            camera.ProcessKeyboard(LEFT, gametime);
        }
    }
    if (keys['d']) {
        if (keys['w']) {
            camera.ProcessKeyboard(RFORWARD, gametime);
        } else if (keys['s']) {
            camera.ProcessKeyboard(RBACKWARD, gametime);
        } else {
            camera.ProcessKeyboard(RIGHT, gametime);
        }
    }
    // coborare
    if(keys['q']){
        camera.ProcessKeyboard(UP, gametime);
    }
    // urcare
    if(keys['e']){
        camera.ProcessKeyboard(DOWN, gametime);
    }
    if(key == '+'){
        camera.increaseSpeed();
    }
    if(key == '-'){
        camera.decreaseSpeed();
    }
    if (key == 27) {
        glutLeaveMainLoop();
    }
    if (key == 'f') {
        glutFullScreen();
    }
}

void ProcessSpecialKeys(int key, int xx, int yy)
{
    // 100-> GLUT_KEY_LEFT, 101-> GLUT_KEY_RIGHT, 102->GLUT_KEY_UP, 103->GLUT_KEY_DOWN
    ProcessNormalKeysDown(key, xx, yy);
    if (keys[GLUT_KEY_UP]) {
        if (keys[GLUT_KEY_RIGHT]) {
            camera.ProcessKeyboard(RFORWARD, gametime);
        } else if (keys[GLUT_KEY_LEFT]) {
            camera.ProcessKeyboard(LFORWARD, gametime);
        } else {
            camera.ProcessKeyboard(FORWARD, gametime);
        }
    }
    if (keys[GLUT_KEY_DOWN]) {
        if (keys[GLUT_KEY_RIGHT]) {
            camera.ProcessKeyboard(RBACKWARD, gametime);
        } else if (keys[GLUT_KEY_LEFT]) {
            camera.ProcessKeyboard(LBACKWARD, gametime);
        } else {
            camera.ProcessKeyboard(BACKWARD, gametime);
        }
    }
    if (keys[GLUT_KEY_LEFT]) {
        if (keys[GLUT_KEY_UP]) {
            camera.ProcessKeyboard(LFORWARD, gametime);
        } else if (keys[GLUT_KEY_DOWN]) {
            camera.ProcessKeyboard(LBACKWARD, gametime);
        } else {
            camera.ProcessKeyboard(LEFT, gametime);
        }
    }
    if (keys[GLUT_KEY_RIGHT]) {
        if (keys[GLUT_KEY_UP]) {
            camera.ProcessKeyboard(RFORWARD, gametime);
        } else if (keys[GLUT_KEY_DOWN]) {
            camera.ProcessKeyboard(RBACKWARD, gametime);
        } else {
            camera.ProcessKeyboard(RIGHT, gametime);
        }
    }

}


int centerX = winWidth / 2, centerY = winHeight / 2;

void UseMouse(int x, int y) {

    float xoffset = (float) x - (float)centerX;
    float yoffset = (float)centerY - (float) y;

    camera.ProcessMouseMovement(xoffset, yoffset);

    if (x != centerX || y != centerY)
        glutWarpPointer(centerX, centerY);

}


//  Crearea si compilarea obiectelor de tip shader;
//	Trebuie sa fie in acelasi director cu proiectul actual;
//  Shaderul de varfuri / vertex shader - afecteaza geometria scenei;
//  Shaderul de fragment / Fragment shader - afecteaza culoarea pixelilor;
void CreateShaders() {
    ProgramId = LoadShaders("shaders/example.vert", "shaders/example.frag");
    sky.CreateSkyShader();
    terrain.CreateTerrainShader();
    trees.CreateTreesShader();
}

//  Se initializeaza un vertex Buffer Object (VBO) pentru tranferul datelor spre memoria placii grafice (spre shadere);
//  In acesta se stocheaza date despre varfuri (coordonate, culori, indici, texturare etc.);
void CreateVBO() {
    //	COORDONATELE varfurilor;
    GLfloat Vertices[] =
            {
                    //baza
                    -50.0f, -50.0f, -50.0f, 1.0f,
                    50.0f, -50.0f, -50.0f, 1.0f,
                    50.0f, 50.0f, -50.0f, 1.0f,
                    -50.0f, 50.0f, -50.0f, 1.0f,
                    //varful
                    25.0f, 25.0f, 50.0f, 1.0f,
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
    for (int i = 0; i < INSTANCE_COUNT; i++) {
        MatModel[i] =
                glm::translate(glm::mat4(1.0f), glm::vec3(200 * sin(2.f * i * 180 / PI),    //	Deplasarea pe OX;
                                                          200 * cos(2.f * i * 180 / PI),    //	Deplasarea pe OY;
                                                          100 *
                                                          i))                                    //	Deplasarea pe OZ;
                * glm::rotate(glm::mat4(1.0f), i * PI / 8, glm::vec3(i, 2 * i * i, i / 3));
    }

    //  Indicii pentru varfuri;
    GLubyte Indices[] =
            {
                    0, 1, 2, 3,  //fata de jos
                    //lateral
                    0, 4, 1, 4, 2, 4, 3, 4, 0,
                    //muchii
                    4, 0, 1, 4, 2, 4, 3, 2, 1, 0, 3
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
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid *) 0);

    //  Se creeaza un buffer pentru CULORI;
    glGenBuffers(1, &VbCol);
    glBindBuffer(GL_ARRAY_BUFFER, VbCol);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Colors), Colors, GL_STATIC_DRAW);
    //	Se activeaza lucrul cu atribute;
    //  Se asociaza atributul (1 = culoare) pentru shader;
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (GLvoid *) 0);
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
                              (void *) (sizeof(glm::vec4) * i));      //  Start offset;
        glVertexAttribDivisor(2 + i, 1);
    }

    //	Se creeaza un buffer pentru INDICI;
    glGenBuffers(1, &EboId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);
}

// Elimina obiectele de tip shader dupa rulare;
void DestroyShaders() {
    glDeleteProgram(ProgramId);
    glDeleteProgram(sky.SkyboxId);
    glDeleteProgram(terrain.TerrainId);
    glDeleteProgram(trees.TreesId);
}

void DestroyVBO() {
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


//  Functia de eliberare a resurselor alocate de program;
void Cleanup() {
    sky.DestroySkyShader();
    terrain.DestroyTerrainShader();
    trees.DestroyTreesShader();
    DestroyShaders();
    sky.DestroySkyboxVBO();
    terrain.DestroyTerrainVBO();
    trees.DestroyTreesVBO();
    DestroyVBO();
}

//  Setarea parametrilor necesari pentru fereastra de vizualizare;
void Initialize() {
    glClearColor(1.f, 1.0f, 1.0f, 0.0f);        //  Culoarea de fond a ecranului;
    CreateVBO();                                //  Trecerea datelor de randare spre bufferul folosit de shadere;
    sky.CreateSkyboxVBO();

    terrain.SetWorldScale(30.0f);
    terrain.CreateTerrainVBO(terrain_width, terrain_depth);
    terrain.SetLightPos(glm::vec3(terrain_width / 2, 
                                    terrain.GetMaxAltitude() * 2, 
                                    terrain_depth / 2));

    // sent without world scale to access the height of the terrain
    trees.CreateTreesVBO(terrain_width, terrain_depth);

    CreateShaders();                            //  Initilizarea shaderelor;

    sky.SkyInit();
    terrain.TerrainInit();
    trees.TreesInit();

    camera.SetPos(glm::vec3(terrain.GetWorldScale() * terrain_width / 2, 
                            terrain.GetWorldScale() * terrain.GetMaxAltitude() / 2,
                            terrain.GetWorldScale() * terrain_depth / 2));
    camera.SetAltitude(terrain.GetWorldScale() * terrain.GetMaxAltitude() / 2);

    glUseProgram(ProgramId);
    //	Instantierea variabilelor uniforme pentru a "comunica" cu shaderele;
    viewLocation = glGetUniformLocation(ProgramId, "view");
    projLocation = glGetUniformLocation(ProgramId, "projection");
    codColLocation = glGetUniformLocation(ProgramId, "codCol");
}

void UpdateTime() {
    float time = glutGet(GLUT_ELAPSED_TIME) * 0.001f;
    delta_t = time - last_time;
    last_time = time;
    gametime += delta_t;
}

//	Functia de desenare a graficii pe ecran;
void RenderFunction() {
    //  Se curata ecranul OpenGL pentru a fi desenat noul continut (bufferul de culoare & adancime);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    UpdateTime();

    // Desenare TERRAIN
    view = camera.GetViewMatrix();
    projection = glm::infinitePerspective(GLfloat(fov), GLfloat(width) / GLfloat(height), zNear);
    model = glm::mat4(1.0f);
    terrain.TerrainRender(view, projection, model, gametime);
    
    view = camera.GetViewMatrix();
    projection = glm::infinitePerspective(GLfloat(fov), GLfloat(width) / GLfloat(height), zNear);
    model = glm::mat4(1.0f);

    trees.TreesRender(view, projection, model, gametime);

    glUseProgram(ProgramId);
    glBindVertexArray(VaoId);
    glBindBuffer(GL_ARRAY_BUFFER, VboId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId);

    // Matricea de vizualizare;
    view = camera.GetViewMatrix();
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

    //	Realizarea proiectiei;
    projection = glm::infinitePerspective(GLfloat(fov), GLfloat(width) / GLfloat(height), zNear);
    glUniformMatrix4fv(projLocation, 1, GL_FALSE, &projection[0][0]);

    //	Desenarea fetelor;
    // codCol = 0;                                                            //  Culoarea;
    //	Transmiterea variabilei uniforme pentru COLORARE spre shadere;
    // glUniform1i(codColLocation, codCol);
    //	Functia glDrawElementsInstanced primeste 4 argumente:
    //	 - arg1 = modul de desenare;
    //	 - arg2 = numarul de varfuri;
    //	 - arg3 = tipul de date al indicilor;
    //	 - arg4 = pointer spre indici (EBO): pozitia de start a indicilor;
    //	 - arg5 = numarul de instante;

    // glDrawElementsInstanced(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_BYTE, (void *) (0), INSTANCE_COUNT);
    // glDrawElementsInstanced(GL_TRIANGLE_STRIP, 9, GL_UNSIGNED_BYTE, (void *) (4), INSTANCE_COUNT);
    //  Desenarea muchiilor;
    // codCol = 1;                                         //	Se schimba culoarea;
    // glUniform1i(codColLocation, codCol);
    // glLineWidth(3.5);
    // glDrawElementsInstanced(GL_LINE_LOOP, 11, GL_UNSIGNED_BYTE, (void *) (13), INSTANCE_COUNT);

    // Desenare SKYBOX
    // Matricea de vizualizare si proiectie;
    view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
    projection = glm::infinitePerspective(GLfloat(fov), GLfloat(width) / GLfloat(height), zNear);
    sky.SkyRender(view, projection, gametime);

    glutSwapBuffers();    //	Inlocuieste imaginea deseneata in fereastra cu cea randata;
    glFlush();            //  Asigura rularea tuturor comenzilor OpenGL apelate anterior;
}

//	Punctul de intrare in program, se ruleaza rutina OpenGL;
int main(int argc, char *argv[]) {
    //  Se initializeaza GLUT si contextul OpenGL si se configureaza fereastra si modul de afisare;

    glutInit(&argc, argv);
    //	Se folosesc 2 buffere pentru desen (unul pentru afisare si unul pentru randare => animatii cursive) si culori RGB + 1 buffer pentru adancime;
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(winWidth, winHeight);                        //  Dimensiunea ferestrei;
    glutInitWindowPosition(100, 100);                                //  Pozitia initiala a ferestrei;
    glutCreateWindow(
            "Forest Walk - OpenGL project");        //	Creeaza fereastra de vizualizare, indicand numele acesteia;

    //	Se initializeaza GLEW si se verifica suportul de extensii OpenGL modern disponibile pe sistemul gazda;
    //  Trebuie initializat inainte de desenare;

    glewInit();

    Initialize();                            //  Setarea parametrilor necesari pentru fereastra de vizualizare;
    glutDisplayFunc(RenderFunction);        //  Desenarea scenei in fereastra;
    glutIdleFunc(RenderFunction);            //	Asigura rularea continua a randarii;
    glutPassiveMotionFunc(UseMouse);
    glutKeyboardUpFunc(ProcessNormalKeysUp);
    glutKeyboardFunc(ProcessNormalKeys);    //	Functii ce proceseaza inputul de la tastatura utilizatorului;
    glutSpecialUpFunc(ProcessSpecialKeysUp);
    glutSpecialFunc(ProcessSpecialKeys);
    glutCloseFunc(Cleanup);                    //  Eliberarea resurselor alocate de program;
    //  Bucla principala de procesare a evenimentelor GLUT (functiile care incep cu glut: glutInit etc.) este pornita;
    //  Prelucreaza evenimentele si deseneaza fereastra OpenGL pana cand utilizatorul o inchide;

    glutMainLoop();

    return 0;
}