#include <GL/glew.h>
#include <GL/freeglut.h>
#include <cmath>
#include <vector>
#include <iostream>

const int N = 10;           // liczba ramion gwiazdy
const float R_OUT = 0.9f;   // promieñ zewnêtrzny
const float R_IN = 0.7f;    // promieñ wewnêtrzny
const int WINDOW_SIZE = 800;

GLuint vao, vbo;
GLuint vboOutline, vaoOutline;
GLuint programFill, programOutline;

void Initialize();
void Display();
void Reshape(int, int);
GLuint CreateShaderProgram(const char*, const char*);
std::vector<float> GenerateStarTriangles(int n, float rOut, float rIn);
std::vector<float> GenerateStarOutline(int n, float rOut, float rIn);


const char* vertexShaderSrc = R"(
#version 330 core
layout(location = 0) in vec2 inPos;
out vec2 vPos;

void main() {
    vPos = inPos;
    gl_Position = vec4(inPos, 0.0, 1.0);
}
)";

const char* fragmentShaderFillSrc = R"(
#version 330 core
in vec2 vPos;
out vec4 outColor;

void main() {
    float dist = length(vPos);
    vec3 colorCenter = vec3(1.0, 0.0, 0.0);
    vec3 colorEdge   = vec3(0.0, 0.0, 0.0);
    vec3 color = mix(colorCenter, colorEdge, dist * 1.5);
    outColor = vec4(color, 1.0);
}
)";

const char* fragmentShaderOutlineSrc = R"(
#version 330 core
out vec4 outColor;
void main() {
    outColor = vec4(1.0, 0.0, 0.0, 1.0);
}
)";

std::vector<float> GenerateStarTriangles(int n, float rOut, float rIn)
{
    std::vector<float> verts;
    std::vector<float> pointsX(2*n), pointsY(2*n);

    for (int i = 0; i < 2 * n; i++) {
        float angle = i * M_PI / n;
        float r = (i % 2 == 0) ? rOut : rIn;
        pointsX[i] = r * cos(angle);
        pointsY[i] = r * sin(angle);
    }

    for (int i = 0; i < 2 * n; i++) {
        int next = (i + 1) % (2 * n);
        verts.push_back(0.0f); verts.push_back(0.0f);
        verts.push_back(pointsX[i]); verts.push_back(pointsY[i]);
        verts.push_back(pointsX[next]); verts.push_back(pointsY[next]);
    }

    return verts;
}

std::vector<float> GenerateStarOutline(int n, float rOut, float rIn)
{
    std::vector<float> outline;
    for (int i = 0; i < 2*n; i++) {
        float angle = i * M_PI / n;
        float r = (i % 2 == 0) ? rOut : rIn;
        outline.push_back(r * cos(angle));
        outline.push_back(r * sin(angle));
    }
    return outline;
}


void Initialize()
{
    std::vector<float> outlineVerts = GenerateStarOutline(N, R_OUT, R_IN);
    glGenVertexArrays(1, &vaoOutline);
    glBindVertexArray(vaoOutline);
    glGenBuffers(1, &vboOutline);
    glBindBuffer(GL_ARRAY_BUFFER, vboOutline);
    glBufferData(GL_ARRAY_BUFFER, outlineVerts.size() * sizeof(float), outlineVerts.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);


    std::vector<float> vertices = GenerateStarTriangles(N, R_OUT, R_IN);
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    programFill = CreateShaderProgram(vertexShaderSrc, fragmentShaderFillSrc);
    programOutline = CreateShaderProgram(vertexShaderSrc, fragmentShaderOutlineSrc);

    glBindVertexArray(0);
    glClearColor(1, 1, 1, 1);
}

void Display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(vao);

    glUseProgram(programFill);
    glDrawArrays(GL_TRIANGLES, 0, N * 6);

    glUseProgram(programOutline);
    glBindVertexArray(vaoOutline);
    glLineWidth(10.0);
    glDrawArrays(GL_LINE_LOOP, 0, N*2);

    glBindVertexArray(0);
    glutSwapBuffers();
}

void Reshape(int w, int h)
{
    glViewport(0, 0, w, h);
}

GLuint CreateShaderProgram(const char* vsSrc, const char* fsSrc)
{
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vsSrc, NULL);
    glCompileShader(vs);

    GLint success;
    glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
    if (!success) { std::cerr << "Vertex shader error\n"; exit(1); }

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fsSrc, NULL);
    glCompileShader(fs);
    glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
    if (!success) { std::cerr << "Fragment shader error\n"; exit(1); }

    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitContextVersion(3, 3);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitWindowSize(WINDOW_SIZE, WINDOW_SIZE);
    glutCreateWindow("lab02zad04");

    glewExperimental = GL_TRUE;
    glewInit();

    Initialize();

    glutDisplayFunc(Display);
    glutReshapeFunc(Reshape);

    glutMainLoop();
    return 0;
}
