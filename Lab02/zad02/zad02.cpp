#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>

GLuint idProgramColor;   // potok do kolorowego rysowania
GLuint idProgramGray;    // potok do odcieni szarości
GLuint idVAO, idVBO;

const int WINDOW_SIZE = 800;

void DisplayScene();
void Initialize();
GLuint CreateShaderProgram(const char* vertexCode, const char* fragmentCode);
void Reshape(int width, int height);

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitContextVersion(3,3);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitWindowSize(WINDOW_SIZE, WINDOW_SIZE);
    glutCreateWindow("lab02zad02");

    glutDisplayFunc(DisplayScene);
    glutReshapeFunc(Reshape);

    glewExperimental = GL_TRUE;
    if(glewInit() != GLEW_OK) { printf("Błąd GLEW!\n"); return -1; }
    if(!GLEW_VERSION_3_3) { printf("Brak OpenGL 3.3!\n"); return -1; }

    Initialize();
    glutMainLoop();

    glDeleteProgram(idProgramColor);
    glDeleteProgram(idProgramGray);
    glDeleteBuffers(1, &idVBO);
    glDeleteVertexArrays(1, &idVAO);

    return 0;
}

// ----------------------------------------------------------
void DisplayScene()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(idVAO);

    // 🔴 Potok 1 — kolorowy
    glUseProgram(idProgramColor);
    glDrawArrays(GL_TRIANGLES, 0, 9); // pierwsze 3 trójkąty (9 wierzchołków)

    // ⚪ Potok 2 — odcienie szarości
    glUseProgram(idProgramGray);
    glDrawArrays(GL_TRIANGLES, 9, 9); // kolejne 3 trójkąty

    glBindVertexArray(0);
    glUseProgram(0);
    glutSwapBuffers();
}

// ----------------------------------------------------------
void Initialize()
{
    // --- tworzymy geometrię ---
    glGenVertexArrays(1, &idVAO);
    glBindVertexArray(idVAO);

    glGenBuffers(1, &idVBO);
    glBindBuffer(GL_ARRAY_BUFFER, idVBO);

    // 6 trójkątów (18 wierzchołków) - po 3 na każdy
    GLfloat vertices[] = {
        // pierwsze 3 trójkąty (kolorowe)
        -0.8f, -0.8f,  // 1
        -0.4f, -0.8f,  // 2
        -0.6f, -0.4f,  // 3

        -0.2f, -0.8f,
         0.2f, -0.8f,
         0.0f, -0.4f,

         0.4f, -0.8f,
         0.8f, -0.8f,
         0.6f, -0.4f,

        // drugie 3 trójkąty (szarość)
        -0.8f, 0.2f,
        -0.4f, 0.2f,
        -0.6f, 0.6f,

        -0.2f, 0.2f,
         0.2f, 0.2f,
         0.0f, 0.6f,

         0.4f, 0.2f,
         0.8f, 0.2f,
         0.6f, 0.6f
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    // --- shadery ---
    //vertex1.glsl
    const char* vertexShaderCode = R"(
        #version 330 core
        layout(location = 0) in vec2 inPosition;
        out vec2 vPos;
        void main() {
            vPos = inPosition;
            gl_Position = vec4(inPosition, 0.0, 1.0);
        }
    )";

    // fragment1.glsl
    const char* fragmentShaderColor = R"(
        #version 330 core
        in vec2 vPos;
        out vec4 outColor;
        void main() {
            float id = float(gl_PrimitiveID);
            float r = (id + 1) / 2;
            float g = (id+1) / 8;
            float b = (id+1) / 1;
            outColor = vec4(r, g, b, 1.0);
        }
    )";

    // fragment2.glsl
    const char* fragmentShaderGray = R"(
        #version 330 core
        in vec2 vPos;
        out vec4 outColor;
        void main() {
            // Generowanie wartości szarości na podstawie pozycji
            float intensity = 0.5;
            outColor = vec4(intensity, intensity, intensity, 1.0);
        }
    )";

    idProgramColor = CreateShaderProgram(vertexShaderCode, fragmentShaderColor);
    idProgramGray  = CreateShaderProgram(vertexShaderCode, fragmentShaderGray);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

// ----------------------------------------------------------
GLuint CreateShaderProgram(const char* vertexCode, const char* fragmentCode)
{
    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vShader, 1, &vertexCode, NULL);
    glCompileShader(vShader);

    GLint status;
    glGetShaderiv(vShader, GL_COMPILE_STATUS, &status);
    if(status != GL_TRUE) {
        printf("Błąd kompilacji vertex shadera!\n");
        exit(1);
    }

    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fShader, 1, &fragmentCode, NULL);
    glCompileShader(fShader);
    glGetShaderiv(fShader, GL_COMPILE_STATUS, &status);
    if(status != GL_TRUE) {
        printf("Błąd kompilacji fragment shadera!\n");
        exit(1);
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vShader);
    glAttachShader(program, fShader);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vShader);
    glDeleteShader(fShader);
    return program;
}

// ----------------------------------------------------------
void Reshape(int width, int height)
{
    glViewport(0, 0, width, height);
}
