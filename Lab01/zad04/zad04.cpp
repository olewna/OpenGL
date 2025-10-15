#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

GLuint idProgramRed;    // potok 1 (odcienie czerwieni)
GLuint idProgramGreen;  // potok 2 (odcienie zieleni)
GLuint idVBO;
GLuint idVAO;

const int N = 6; // dwa trójkąty = 6 wierzchołków
const int WINDOW_SIZE = 800;

// funkcje
void DisplayScene();
void Initialize();
GLuint LoadShaderProgram(const char* vertexFile, const char* fragmentFile);
char* LoadTextFile(const char* filename);
void Reshape(int width, int height);

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitContextVersion(3, 3);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitWindowSize(WINDOW_SIZE, WINDOW_SIZE);
    glutCreateWindow("Zad04");

    glutDisplayFunc(DisplayScene);
    glutReshapeFunc(Reshape);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        printf("Blad inicjalizacji GLEW!\n");
        exit(1);
    }

    Initialize();
    glutMainLoop();
    return 0;
}

void DisplayScene()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glBindVertexArray(idVAO);

    // 🔴 pierwszy potok — czerwone trójkąty
    glUseProgram(idProgramRed);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // 🟢 drugi potok — zielone trójkąty
    glUseProgram(idProgramGreen);
    glDrawArrays(GL_TRIANGLES, 3, 3);

    glUseProgram(0);
    glBindVertexArray(0);

    glutSwapBuffers();
}

void Initialize()
{
    srand(time(NULL));

    GLfloat triangles[] = {
        // trójkąt 1 — po lewej
        -0.8f, -0.5f,
         0.0f, -0.5f,
        -0.4f,  0.5f,

        // trójkąt 2 — po prawej
         0.2f, -0.5f,
         0.8f, -0.5f,
         0.5f,  0.5f,
    };

    glGenVertexArrays(1, &idVAO);
    glBindVertexArray(idVAO);

    glGenBuffers(1, &idVBO);
    glBindBuffer(GL_ARRAY_BUFFER, idVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangles), triangles, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    idProgramRed   = LoadShaderProgram("vertex-1.glsl", "fragment-1.glsl");
    idProgramGreen = LoadShaderProgram("vertex-2.glsl", "fragment-2.glsl");

    glClearColor(1, 1, 1, 1.0f);
}

GLuint LoadShaderProgram(const char* vertexFile, const char* fragmentFile)
{
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    char* vertexCode = LoadTextFile(vertexFile);
    char* fragmentCode = LoadTextFile(fragmentFile);

    glShaderSource(vertexShader, 1, (const GLchar**)&vertexCode, NULL);
    glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentCode, NULL);
    glCompileShader(vertexShader);
    glCompileShader(fragmentShader);

    GLint status;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) printf("Blad kompilacji vertex shadera: %s\n", vertexFile);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) printf("Blad kompilacji fragment shadera: %s\n", fragmentFile);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    free(vertexCode);
    free(fragmentCode);

    return program;
}

char* LoadTextFile(const char* filename)
{
    FILE* file = fopen(filename, "rb");
    if (!file) { printf("Nie moge otworzyc pliku: %s\n", filename); exit(1); }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    char* buffer = (char*)malloc(size + 1);
    fread(buffer, 1, size, file);
    buffer[size] = '\0';
    fclose(file);
    return buffer;
}

void Reshape(int width, int height)
{
    glViewport(0, 0, width, height);
}
