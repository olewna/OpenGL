#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

GLuint idProgram;
GLuint idVBO;
GLuint idVAO;

const int N = 100; // liczba punktów
const int WINDOW_SIZE = 800;

void DisplayScene();
void Initialize();
void CreateVertexShader();
void CreateFragmentShader();
void Reshape(int width, int height);

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitContextVersion(3, 3);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitWindowSize(WINDOW_SIZE, WINDOW_SIZE);
    glutCreateWindow("zad03");

    glutDisplayFunc(DisplayScene);
    glutReshapeFunc(Reshape);

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
    glBindVertexArray(idVAO);
    glDrawArrays(GL_POINTS, 0, N);
    glBindVertexArray(0);
    glUseProgram(0);

    glutSwapBuffers();
}

void Initialize()
{
    srand(time(NULL));

    // tablice wierzcho³ków i kolorów
    GLfloat vertices[2 * N];
    GLfloat colors[3 * N];
    GLfloat sizes[N];

    for (int i = 0; i < N; ++i)
    {
        // pierwsza pozycja to x, druga to y
        // pozycja w zakresie [-1, 1]
        vertices[2 * i] = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;     // X
        vertices[2 * i + 1] = ((float)rand() / RAND_MAX) * 2.0f - 1.0f; // Y

        // kolor (losowy RGB)
        colors[3 * i] = (float)rand() / RAND_MAX;
        colors[3 * i + 1] = (float)rand() / RAND_MAX;
        colors[3 * i + 2] = (float)rand() / RAND_MAX;

        // rozmiar punktu (np. od 2 do 20)
        //sizes[i] = 10.0;
         sizes[i] = ((float)rand() / RAND_MAX) * 18.0f + 2.0f;
    }

    // VAO
    glGenVertexArrays(1, &idVAO);
    glBindVertexArray(idVAO);

    // VBO — pozycje, kolory i rozmiary razem
    GLuint vbos[3];
    glGenBuffers(3, vbos);

    // Pozycje
    glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    // Kolory
    glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);

    // Rozmiary
    glBindBuffer(GL_ARRAY_BUFFER, vbos[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sizes), sizes, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    // Shadery
    idProgram = glCreateProgram();
    CreateVertexShader();
    CreateFragmentShader();

    glLinkProgram(idProgram);
    glValidateProgram(idProgram);

    // Aktywuj mo¿liwoœæ zmiany rozmiaru punktów
    glEnable(GL_PROGRAM_POINT_SIZE);

    // Ustaw kolor t³a
    glClearColor(1, 1, 1, 1.0f);
}

void CreateVertexShader()
{
    GLuint shader = glCreateShader(GL_VERTEX_SHADER);

    const GLchar* code =
        "#version 330 core\n"
        "layout(location = 0) in vec2 inPosition;"
        "layout(location = 1) in vec3 inColor;"
        "layout(location = 2) in float inSize;"
        "out vec3 fragColor;"
        "void main() {"
        "   gl_Position = vec4(inPosition, 0.0, 1.0);"
        "   fragColor = inColor;"
        "   gl_PointSize = inSize;"
        "}";

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

void CreateFragmentShader()
{
    GLuint shader = glCreateShader(GL_FRAGMENT_SHADER);

    const GLchar* code =
        "#version 330 core\n"
        "in vec3 fragColor;"
        "out vec4 outColor;"
        "void main() {"
        "   outColor = vec4(fragColor, 1.0);"
        "}";

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

void Reshape(int width, int height)
{
    glViewport(0, 0, width, height);
}
