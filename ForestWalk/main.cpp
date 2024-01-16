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
#include "Water/water.cpp"

//	Numarul de multiplicari 
#define INSTANCE_COUNT 10

//  Identificatorii obiectelor de tip OpenGL;
GLint
        ProgramId;
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
    if (keys['q']) {
        camera.ProcessKeyboard(UP, gametime);
    }
    // urcare
    if (keys['e']) {
        camera.ProcessKeyboard(DOWN, gametime);
    }
    if (key == '+') {
        camera.increaseSpeed();
    }
    if (key == '-') {
        camera.decreaseSpeed();
    }
    if (key == 27) {
        glutLeaveMainLoop();
    }
    if (key == 'f') {
        glutFullScreen();
    }
}

void ProcessSpecialKeys(int key, int xx, int yy) {
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

    float xoffset = (float) x - (float) centerX;
    float yoffset = (float) centerY - (float) y;

    camera.ProcessMouseMovement(xoffset, yoffset);

    if (x != centerX || y != centerY)
        glutWarpPointer(centerX, centerY);

}

//  Crearea si compilarea obiectelor de tip shader;
//	Trebuie sa fie in acelasi director cu proiectul actual;
//  Shaderul de varfuri / vertex shader - afecteaza geometria scenei;
//  Shaderul de fragment / Fragment shader - afecteaza culoarea pixelilor;
void CreateShaders() {
    sky.CreateSkyShader();
    terrain.CreateTerrainShader();
    CreateWaterShader();
}

// Elimina obiectele de tip shader dupa rulare;
void DestroyShaders() {
    glDeleteProgram(ProgramId);
    glDeleteProgram(sky.SkyboxId);
    glDeleteProgram(terrain.TerrainId);
    glDeleteProgram(WaterId);
}

//  Functia de eliberare a resurselor alocate de program;
void Cleanup() {
    sky.DestroySkyShader();
    terrain.DestroyTerrainShader();
    sky.DestroySkyboxVBO();
    terrain.DestroyTerrainVBO();
    DestroyWaterShader();
    DestroyWaterVBO();
    DestroyShaders();
}

//  Setarea parametrilor necesari pentru fereastra de vizualizare;
void Initialize() {
    glClearColor(1.f, 1.0f, 1.0f, 0.0f);        //  Culoarea de fond a ecranului;

    sky.CreateSkyboxVBO();

    terrain.SetWorldScale(30.0f);
    terrain.CreateTerrainVBO(terrain_width, terrain_depth);
    terrain.SetLightPos(glm::vec3(terrain_width / 2,
                                  terrain.GetMaxAltitude() * 2,
                                  terrain_depth / 2));
    CreateWaterVBO((int)terrain.t_width , (int)terrain.GetWorldScale());

    CreateShaders();                            //  Initilizarea shaderelor;

    sky.SkyInit();
    terrain.TerrainInit();
    WaterInit();

    camera.SetPos(glm::vec3(terrain.GetWorldScale() * terrain_width / 2,
                            terrain.GetWorldScale() * terrain.GetMaxAltitude() / 2,
                            terrain.GetWorldScale() * terrain_depth / 2));
    camera.SetAltitude(terrain.GetWorldScale() * terrain.GetMaxAltitude() / 2);

    glEnable( GL_BLEND );
//    glPolygonMode(GL_FRONT, GL_LINE);
//    glPolygonMode(GL_BACK, GL_LINE);

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
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    view = camera.GetViewMatrix();
    projection = glm::infinitePerspective(GLfloat(fov), GLfloat(width) / GLfloat(height), zNear);
    model = glm::mat4(1.0f);
    WaterRender(view, projection, model, gametime);
    glUniform3f(glGetUniformLocation(WaterId, "cameraPos"), camera.Position.x,camera.Position.y,camera.Position.z);
    glUniform1i(glGetUniformLocation(WaterId, "skyboxday"), 0);
    glUniform1i(glGetUniformLocation(WaterId, "skyboxnight"), 1);
    glDisable(GL_BLEND);
    glCullFace(GL_BACK);

    // Desenare TERRAIN
    view = camera.GetViewMatrix();
    projection = glm::infinitePerspective(GLfloat(fov), GLfloat(width) / GLfloat(height), zNear);
    model = glm::mat4(1.0f);
    terrain.TerrainRender(view, projection, model, gametime);
    glDisable(GL_CULL_FACE);

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
    glutCreateWindow("Forest Walk - OpenGL project");
    //	Creeaza fereastra de vizualizare, indicand numele acesteia;

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