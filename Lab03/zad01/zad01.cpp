// pocz¹tek, inicjacja bibliotek
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include "co-to-jest.h"

GLuint program;   // potok
GLuint VAO, VBO, EBO;

const int WINDOW_SIZE = 800;

// deklarowanie funkcji
void DisplayScene();
void Initialize();
void CreateFragmentShader(void);
void CreateVertexShader(void);
//GLuint CreateShaderProgram(const char* vertexCode, const char* fragmentCode);
void Reshape(int width, int height);

int main(int argc, char *argv[])
{
    // tworzenie okienka itp
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitContextVersion(3,3);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitWindowSize(WINDOW_SIZE, WINDOW_SIZE);
    glutCreateWindow("nazwa okienka");

    glutDisplayFunc(DisplayScene);
    glutReshapeFunc(Reshape);

    glewExperimental = GL_TRUE;
    if(glewInit() != GLEW_OK) { printf("B³¹d GLEW!\n"); return -1; }
    if(!GLEW_VERSION_3_3) { printf("Brak OpenGL 3.3!\n"); return -1; }

    //funkcja inicjalizuj¹cca i nieskonczona petla zeby program dzia³a³
    Initialize();
    glutMainLoop();

    // czyszczenie
    glDeleteProgram(program);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);

    return 0;
}

void DisplayScene()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(program);
        glBindVertexArray(VAO);
        // glDrawArrays(GL_TRIANGLES, 0, NUMBER_OF_VERTICES);
        glDrawElements(GL_TRIANGLES, 137*3, GL_UNSIGNED_INT, NULL);
    glBindVertexArray(0);
    glUseProgram(0);

    glutSwapBuffers();
}

void Initialize()
{
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // wierzcho³ki VBO
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(coord), coord, GL_STATIC_DRAW);

    // EBO indeksy
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // atrybut pozycji
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    program = glCreateProgram();
    CreateVertexShader();
    CreateFragmentShader();
    glLinkProgram(program);

    glValidateProgram(program);

    glClearColor(1, 1, 1, 1);
}

void CreateVertexShader(void)
{
    GLuint shader = glCreateShader(GL_VERTEX_SHADER);

    const GLchar *code =R"(
        #version 330 core
        layout(location = 0) in vec2 inPosition;
        out vec3 fragColor;

        void main() {
            vec3 pos = vec3(inPosition, 0.0);

            fragColor = vec3((pos.x + 1.0) * 0.5, (pos.y + 1.0) * 0.5, 0.5);

            gl_Position = vec4(pos.xy, 0.0, 1.0);
        }
    )";

    glShaderSource(shader, 1, &code, NULL);
    glCompileShader(shader);

    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) { printf("Blad kompilacji vertex shadera!\n"); exit(1);}
    glAttachShader(program, shader);
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
    if (status != GL_TRUE) { printf("Blad kompilacji fragment shadera!\n"); exit(1); }
    glAttachShader(program, shader);
}

void Reshape(int width, int height)
{
    glViewport(0, 0, width, height);
}
