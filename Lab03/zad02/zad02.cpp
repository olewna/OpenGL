#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cmath>
#include <GL/glew.h>
#include <GL/freeglut.h>


#include "utilities.hpp"
#include "house.h"

GLuint idProgram;
GLuint idVBO;
GLuint idVAO;

const int WINDOW_SIZE = 800;

float rotationX = 0.0f;
float rotationY = 0.0f;
float rotationZ = 0.0f;


// funkcje
void DisplayScene();
void Initialize();
void Reshape( int width, int height );
void Keyboard(unsigned char key, int x, int y);

void MultiplyMatrix(float out[16], const float a[16], const float b[16]);


int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitContextVersion(3, 3);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitWindowSize(WINDOW_SIZE, WINDOW_SIZE);
    glutCreateWindow("lab03zad02");

    glutDisplayFunc(DisplayScene);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Keyboard);

    glewExperimental = GL_TRUE;
    if (GLEW_OK != glewInit()){ printf("GLEW Error\n"); exit(1);}
    if (!GLEW_VERSION_3_3){ printf("Brak OpenGL 3.3!\n"); exit(1);}

    Initialize();
    glutMainLoop();

    glDeleteProgram(idProgram);
    glDeleteBuffers(1, &idVBO);
    glDeleteVertexArrays(1, &idVAO);

    return 0;
}

void DisplayScene()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(idProgram);

   // Macierze rotacji wokół osi X, Y, Z
    float cosX = cos(rotationX);
    float sinX = sin(rotationX);
    float cosY = cos(rotationY);
    float sinY = sin(rotationY);
    float cosZ = cos(rotationZ);
    float sinZ = sin(rotationZ);

    float rotX[16] = {
        1, 0,      0,     0,
        0, cosX,  -sinX,  0,
        0, sinX,   cosX,  0,
        0, 0,      0,     1
    };

    float rotY[16] = {
        cosY, 0, sinY, 0,
        0,    1, 0,    0,
        -sinY,0, cosY, 0,
        0,    0, 0,    1
    };

    float rotZ[16] = {
        cosZ, -sinZ, 0, 0,
        sinZ,  cosZ, 0, 0,
        0,      0,   1, 0,
        0,      0,   0, 1
    };

    // rotZ * rotY * rotX
    float temp[16], finalRot[16];
    MultiplyMatrix(temp, rotZ, rotY);
    MultiplyMatrix(finalRot, temp, rotX);

    GLuint loc = glGetUniformLocation(idProgram, "rotationMatrix");
    glUniformMatrix4fv(loc, 1, GL_FALSE, finalRot);

    glBindVertexArray(idVAO);

    glDrawArrays(GL_TRIANGLES, 0, NUMBER_OF_VERTICES);

    glBindVertexArray(0);
    glUseProgram(0);

    glutSwapBuffers();
}

void Initialize()
{
    glGenVertexArrays(1, &idVAO);
    glBindVertexArray(idVAO);

    glGenBuffers(1, &idVBO);
    glBindBuffer(GL_ARRAY_BUFFER, idVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Mesh_Vertices), Mesh_Vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    GLfloat Mesh_Colors[NUMBER_OF_VERTICES * 3] = {
    // 1. Trójk¹t – czerwony dach
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,

    // 2. Trójk¹t – pomarañczowy
    1.0f, 0.5f, 0.0f,
    1.0f, 0.5f, 0.0f,
    1.0f, 0.5f, 0.0f,

    // 3. Trójk¹t – ¿ó³ty
    1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,

    // 4. Trójk¹t – jasnozielony
    0.4f, 1.0f, 0.4f,
    0.4f, 1.0f, 0.4f,
    0.4f, 1.0f, 0.4f,

    // 5. Trójk¹t – ciemnozielony
    0.0f, 0.6f, 0.2f,
    0.0f, 0.6f, 0.2f,
    0.0f, 0.6f, 0.2f,

    // 6. Trójk¹t – niebieski
    0.0f, 0.3f, 1.0f,
    0.0f, 0.3f, 1.0f,
    0.0f, 0.3f, 1.0f,

    // 7. Trójk¹t – fioletowy
    0.6f, 0.2f, 0.8f,
    0.6f, 0.2f, 0.8f,
    0.6f, 0.2f, 0.8f,

    // 8. Trójk¹t – br¹zowy
    0.5f, 0.3f, 0.1f,
    0.5f, 0.3f, 0.1f,
    0.5f, 0.3f, 0.1f,

    // 9. Trójk¹t – szary
    0.6f, 0.6f, 0.6f,
    0.6f, 0.6f, 0.6f,
    0.6f, 0.6f, 0.6f
};

    GLuint idVBO_colors;
    glGenBuffers(1, &idVBO_colors);
    glBindBuffer(GL_ARRAY_BUFFER, idVBO_colors);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Mesh_Colors), Mesh_Colors, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);

    idProgram = glCreateProgram();
	glAttachShader( idProgram, LoadShader(GL_VERTEX_SHADER, "vertex.glsl"));
	glAttachShader( idProgram, LoadShader(GL_FRAGMENT_SHADER, "fragment.glsl"));
	LinkAndValidateProgram( idProgram );
    glLinkProgram(idProgram);

    glValidateProgram(idProgram);

    glClearColor(1, 1, 1, 1.0f);
}

void Keyboard(unsigned char key, int x, int y) {
    switch(key) {
        case 'a': rotationY += 0.1f; break; // w lewo
        case 'd': rotationY -= 0.1f; break; // w prawo
        case 'w': rotationX += 0.1f; break; // w górę
        case 's': rotationX -= 0.1f; break; // w dół
        case 'e': rotationZ += 0.1f; break;
        case 'q': rotationZ -= 0.1f; break;
        case 27: exit(0); break; // ESC
    }
    glutPostRedisplay();
}

void Reshape(int width, int height)
{
    glViewport(0, 0, width, height);
}

void MultiplyMatrix(float out[16], const float a[16], const float b[16])
{
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++) {
            out[i * 4 + j] = 0;
            for (int k = 0; k < 4; k++)
                out[i * 4 + j] += a[i * 4 + k] * b[k * 4 + j];
        }
}
