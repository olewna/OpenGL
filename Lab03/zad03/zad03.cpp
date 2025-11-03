#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include "utilities.hpp"

#define idx idx_A
#define coord coord_A
#include "obiektA.h"
#undef idx
#undef coord

#define idx idx_B
#define coord coord_B
#include "obiektB.h"
#undef idx
#undef coord

GLuint idProgram;
GLuint idVBOa, idVAOa, idEBOa;
GLuint idVBOb, idVAOb, idEBOb;

const int WINDOW_SIZE = 800;

float scale = 1.0f;
float rotationZ = 0.0f;
float centerX, centerY = 0.0f;


// funkcje
void DisplayScene();
void Initialize();
void Reshape( int width, int height );
void Keyboard(unsigned char key, int x, int y);

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitContextVersion(3, 3);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitWindowSize(WINDOW_SIZE, WINDOW_SIZE);
    glutCreateWindow("lab03zad03");

    glutDisplayFunc(DisplayScene);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Keyboard);

    glewExperimental = GL_TRUE;
    if (GLEW_OK != glewInit()){ printf("GLEW Error\n"); exit(1);}
    if (!GLEW_VERSION_3_3){ printf("Brak OpenGL 3.3!\n"); exit(1);}

    Initialize();
    glutMainLoop();

    glDeleteProgram(idProgram);
    glDeleteBuffers(1, &idVBOa);
    glDeleteBuffers(1, &idVBOb);
    glDeleteVertexArrays(1, &idVAOa);
    glDeleteVertexArrays(1, &idVAOb);

    return 0;
}

void DisplayScene()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(idProgram);

    // --- Figura B, kopia 1 ---
    glm::mat4 modelB1 = glm::mat4(1.0f);
    modelB1 = glm::translate(modelB1, glm::vec3(0.0f, -0.5f, 0.0f));
    modelB1 = glm::scale(modelB1, glm::vec3(0.5f, 0.5f, 1.0f));
    glUniformMatrix4fv(glGetUniformLocation(idProgram, "model"), 1, GL_FALSE, &modelB1[0][0]);
    glBindVertexArray(idVAOb);
    glDrawElements(GL_TRIANGLES, sizeof(idx_B)/sizeof(GLuint), GL_UNSIGNED_INT, 0);

    // --- Figura A ---
    glm::mat4 modelA = glm::mat4(1.0f); // identyczna pozycja
//    modelA = glm::translate(modelA, glm::vec3(0.0f, -0.5f, 0.0f));
    modelA = glm::rotate(modelA, rotationZ, glm::vec3(0.0f, 0.0f, 1.0f));
    glUniformMatrix4fv(glGetUniformLocation(idProgram, "model"), 1, GL_FALSE, &modelA[0][0]);
    glBindVertexArray(idVAOa);
    glDrawElements(GL_TRIANGLES, sizeof(idx_A)/sizeof(GLuint), GL_UNSIGNED_INT, 0);

    // --- Figura B, kopia 2 ---
    glm::mat4 modelB2 = glm::mat4(1.0f);
    modelB2= glm::translate(modelB2, glm::vec3(centerX, centerY, 0.0f));
    modelB2 = glm::scale(modelB2, glm::vec3(scale, scale, 1.0f));
    modelB2 = glm::translate(modelB2, glm::vec3(-centerX, -centerY, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(idProgram, "model"), 1, GL_FALSE, &modelB2[0][0]);
    glBindVertexArray(idVAOb);
    glDrawElements(GL_TRIANGLES, sizeof(idx_B)/sizeof(GLuint), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glUseProgram(0);

    glutSwapBuffers();
}

void Initialize()
{
    glGenVertexArrays(1, &idVAOa);
    glBindVertexArray(idVAOa);

    glGenBuffers(1, &idVBOa);
    glBindBuffer(GL_ARRAY_BUFFER, idVBOa);
    glBufferData(GL_ARRAY_BUFFER, sizeof(coord_A), coord_A, GL_STATIC_DRAW);

    glGenBuffers(1, &idEBOa);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idEBOa);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx_A), idx_A, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    glGenVertexArrays(1, &idVAOb);
    glBindVertexArray(idVAOb);

    glGenBuffers(1, &idVBOb);
    glBindBuffer(GL_ARRAY_BUFFER, idVBOb);
    glBufferData(GL_ARRAY_BUFFER, sizeof(coord_B), coord_B, GL_STATIC_DRAW);

    glGenBuffers(1, &idEBOb);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idEBOb);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx_B), idx_B, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    int numVerts = sizeof(coord_B)/sizeof(coord_B[0])/2; // 2 bo x i y

    for (int i = 0; i < numVerts; ++i) {
        centerX += coord_B[2*i];
        centerY += coord_B[2*i + 1];
        printf("Center: X = %f, Y = %f\n", centerX, centerY);
    }

    centerX /= numVerts;
    centerY /= numVerts;
    printf("Center: X = %f, Y = %f\n", centerX, centerY);

    idProgram = glCreateProgram();
	glAttachShader( idProgram, LoadShader(GL_VERTEX_SHADER, "vertex.glsl"));
	glAttachShader( idProgram, LoadShader(GL_FRAGMENT_SHADER, "fragment.glsl"));
	LinkAndValidateProgram( idProgram );
    glLinkProgram(idProgram);

    glValidateProgram(idProgram);

    glClearColor(1, 1, 1, 1);
}

void Keyboard(unsigned char key, int x, int y) {
    switch(key) {
        case 'w': scale += 0.1f; break; // skalowanie
        case 's':
            if(scale > 0.1f)
                scale -= 0.1f;
            if(scale < 0.0f)
                scale = 0.1f;
            break;
        case 'q': rotationZ += 0.1f; break; // obracanie
        case 'e': rotationZ -= 0.1f; break;
        case 27: exit(0); break; // ESC
    }
    glutPostRedisplay();
}

void Reshape(int width, int height)
{
    glViewport(0, 0, width, height);
}
