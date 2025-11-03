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
#include "obiektB.h"

GLuint idProgram;
GLuint idVBO, idVAO, idEBO;

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;


float scale = 1.0f;
float rotationZ = 0.0f;


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
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("lab03zad04");

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

    int NEW_WINDOW_WIDTH = WINDOW_WIDTH/ 2;
    int NEW_WINDOW_HEIGHT= WINDOW_HEIGHT/ 2;

    glUseProgram(idProgram);

    for(int i = 0; i < 2; i++) {
        for(int j = 0; j < 2; j++) {
        glViewport(i * NEW_WINDOW_WIDTH, j * NEW_WINDOW_HEIGHT, NEW_WINDOW_WIDTH, NEW_WINDOW_HEIGHT);

        int mode = i*2 +j;
        glUniform1i(glGetUniformLocation(idProgram, "Mode"), mode);

    //    glm::mat4 modelB1 = glm::mat4(1.0f);
    //    modelB1 = glm::translate(modelB1, glm::vec3(0.0f, -0.5f, 0.0f));
    //    modelB1 = glm::scale(modelB1, glm::vec3(0.5f, 0.5f, 1.0f));
    //    glUniformMatrix4fv(glGetUniformLocation(idProgram, "model"), 1, GL_FALSE, &modelB1[0][0]);

        glBindVertexArray(idVAO);
        glDrawElements(GL_TRIANGLES, sizeof(idx)/sizeof(GLuint), GL_UNSIGNED_INT, 0);

        }
    }

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
    glBufferData(GL_ARRAY_BUFFER, sizeof(coord), coord, GL_STATIC_DRAW);


    glGenBuffers(1, &idEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx), idx, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

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
