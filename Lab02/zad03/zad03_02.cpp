#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "house.h"

GLuint idProgram;
GLuint idVBO;
GLuint idVAO;

const int WINDOW_SIZE = 800;

float rotationX= 0.0f;
float rotationY = 0.0f;
GLint locRotationY, locRotationX;
float posX = 0.0f;
float posY = 0.0f;
GLint locOffsetX, locOffsetY;

// funkcje
void DisplayScene();
void Initialize();
void CreateVertexShader( void );
void CreateFragmentShader( void );
void Reshape( int width, int height );
void Keyboard(unsigned char key, int x, int y);


int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitContextVersion(3, 3);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitWindowSize(WINDOW_SIZE, WINDOW_SIZE);
    glutCreateWindow("lab02Zad03");

    glutDisplayFunc(DisplayScene);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Keyboard);

    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        printf("GLEW Error\n");
        exit(1);
    }

    if (!GLEW_VERSION_3_3)
    {
        printf("Brak OpenGL 3.3!\n");
        exit(1);
    }

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

    glUniform1f(locRotationY, rotationY);
    glUniform1f(locRotationX, rotationX);
    glUniform1f(locOffsetX, posX);
    glUniform1f(locOffsetY, posY);

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

    idProgram = glCreateProgram();
    CreateVertexShader();
    CreateFragmentShader();
    glLinkProgram(idProgram);

    locRotationY = glGetUniformLocation(idProgram, "rotationY");
    locRotationX = glGetUniformLocation(idProgram, "rotationX");
    locOffsetX = glGetUniformLocation(idProgram, "offsetX");
    locOffsetY = glGetUniformLocation(idProgram, "offsetY");


    glValidateProgram(idProgram);

    glClearColor(1, 1, 1, 1.0f);
}

void CreateVertexShader(void)
{
    GLuint shader = glCreateShader(GL_VERTEX_SHADER);

    const GLchar *code =R"(
        #version 330 core
        layout(location = 0) in vec2 inPosition;
        uniform float rotationY; //lewo prawo
        uniform float rotationX; // góra dół
        uniform float offsetX;
        uniform float offsetY;
        out vec3 fragColor;

        void main() {
            vec2 position = vec2(inPosition.y, -inPosition.x);
            vec3 pos = vec3(position, 0.0);

            mat3 rotY = mat3(
                             cos(rotationY), 0.0, sin(rotationY),
                             0.0, 1.0, 0.0,
                             -sin(rotationY), 0.0, cos(rotationY));

            mat3 rotX = mat3(
                             1.0, 0.0, 0.0,
                             0.0, cos(rotationX), -sin(rotationX),
                             0.0, sin(rotationX), cos(rotationX));

            pos = rotY * rotX * pos;

            pos.xy += vec2(offsetX, offsetY);

            fragColor = vec3((pos.x + 1.0) * 0.5, (pos.y + 1.0) * 0.5, 0.5);

            gl_Position = vec4(pos.xy, 0.0, 1.0);
        }
    )";

    glShaderSource(shader, 1, &code, NULL);
    glCompileShader(shader);

    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE)
    {
        printf("Blad kompilacji vertex shadera!\n");
        exit(1);
    }
    glAttachShader(idProgram, shader);
}

void CreateFragmentShader(void)
{
    GLuint shader = glCreateShader(GL_FRAGMENT_SHADER);

    const GLchar *code = R"(
        #version 330 core
        in vec3 fragColor;
        out vec4 outColor;

        void main() {
            vec2 uv = gl_FragCoord.xy / vec2(500.0, 500.0);
            float r = fract(sin(float(gl_PrimitiveID) * 123.9898) * 91111.5453);
            float g = fract(sin(float(gl_PrimitiveID) * 72.233) * 94599.6789);
            float b = fract(sin(float(gl_PrimitiveID) * 45.164) * 84567.4321);
            vec3 baseColor = vec3(r, g, b);
            baseColor *= 0.5;
            //outColor = vec4(baseColor, 1.0);

            float triID = float(gl_PrimitiveID);

            float idFactor = fract(sin(triID * 91.23) * 43758.5453);

            vec3 finalColor = fragColor * (0.6 + 0.4 * idFactor);
            outColor = vec4(finalColor, 1.0);
        }
        )";

    glShaderSource(shader, 1, &code, NULL);
    glCompileShader(shader);

    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE)
    {
        printf("Blad kompilacji fragment shadera!\n");
        exit(1);
    }
    glAttachShader(idProgram, shader);
}

void Keyboard(unsigned char key, int x, int y) {
    switch(key) {
        //case 'a': rotationY += 0.1f; break; // w lewo
        //case 'd': rotationY -= 0.1f; break; // w prawo
        case 'w': posY += 0.05f; break; // w górę
        case 's': posY -= 0.05f; break; // w dół
        case 'a': posX -= 0.05f; break; // w lewo
        case 'd': posX += 0.05f; break;
        case 27: exit(0); break; // ESC
    }
    glutPostRedisplay();
}

void Reshape(int width, int height)
{
    glViewport(0, 0, width, height);
}
