#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

glm::mat4 matProj, matView;

#include "utilities.hpp"
#include "objloader.hpp"

// =======================================================
// 1. Klasa do obslugi potoku
// =======================================================
class CProgram
{
public:
    CProgram() : idProgram(0) {}
    ~CProgram() { Clean(); }

    bool Init()
    {
        idProgram = glCreateProgram();
        return idProgram != 0;
    }

    void Clean()
    {
        if (idProgram)
        {
            glDeleteProgram(idProgram);
            idProgram = 0;
        }
    }

    bool LoadShaders(const char *vertexFile, const char *fragmentFile)
    {
        glAttachShader(idProgram, LoadShader(GL_VERTEX_SHADER, vertexFile));
        glAttachShader(idProgram, LoadShader(GL_FRAGMENT_SHADER, fragmentFile));
        LinkAndValidateProgram(idProgram);
        return true;
    }

    void Use() { glUseProgram(idProgram); }
    void UnUse() { glUseProgram(0); }

    void SetMat4(const char *name, const glm::mat4 &matrix)
    {
        glUniformMatrix4fv(glGetUniformLocation(idProgram, name), 1, GL_FALSE, glm::value_ptr(matrix));
    }
    void SetInt(const char *name, int value)
    {
        glUniform1i(glGetUniformLocation(idProgram, name), value);
    }

    void SetFloat(const char *name, float value)
    {
        glUniform1f(glGetUniformLocation(idProgram, name), value);
    }

    GLuint GetId() const { return idProgram; }

private:
    GLuint idProgram;
};

// =======================================================
// 2. Oraz druga klasa do obslugi obiektow 3D
// =======================================================
class CMesh
{
public:
    CMesh() : idVAO(0), idVBO_coords(0), idVBO_uvs(0), idVBO_normals(0), idTexture(0), vertexCount(0), matModel(1.0f) {}
    ~CMesh() { Clean(); }

    bool CreateFromOBJ(const char *filename)
    {
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec2> uvs;
        std::vector<glm::vec3> normals;

        if (!loadOBJ(filename, vertices, uvs, normals))
        {
            std::cerr << "OBJ file error: " << filename << std::endl;
            return false;
        }

        vertexCount = vertices.size();

        glGenVertexArrays(1, &idVAO);
        glBindVertexArray(idVAO);

        glGenBuffers(1, &idVBO_coords);
        glBindBuffer(GL_ARRAY_BUFFER, idVBO_coords);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

        if (!uvs.empty())
        {
            glGenBuffers(1, &idVBO_uvs);
            glBindBuffer(GL_ARRAY_BUFFER, idVBO_uvs);
            glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), uvs.data(), GL_STATIC_DRAW);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);
        }

        if (!normals.empty())
        {
            glGenBuffers(1, &idVBO_normals);
            glBindBuffer(GL_ARRAY_BUFFER, idVBO_normals);
            glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
        }

        glBindVertexArray(0);
        return true;
    }

    void LoadTexture(const char *filename)
    {
        int texWidth, texHeight, texChannels;
        stbi_set_flip_vertically_on_load(true);
        unsigned char *data = stbi_load(filename, &texWidth, &texHeight, &texChannels, 0);

        if (!data)
        {
            printf("Texture can’t be loaded!\n");
            return;
        }

        glGenTextures(1, &idTexture);
        glBindTexture(GL_TEXTURE_2D, idTexture);

        GLenum internalFormat;
        GLenum format;
        if (texChannels == 1)
        {
            format = GL_RED;
            internalFormat = GL_R8;
        }
        else if (texChannels == 3)
        {
            format = GL_RGB;
            internalFormat = GL_RGB8;
        }
        else if (texChannels == 4)
        {
            format = GL_RGBA;
            internalFormat = GL_RGBA8;
        }

        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, texWidth, texHeight, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        stbi_image_free(data);
    }

    void Clean()
    {
        if (idVBO_coords)
            glDeleteBuffers(1, &idVBO_coords);
        if (idVBO_uvs)
            glDeleteBuffers(1, &idVBO_uvs);
        if (idVBO_normals)
            glDeleteBuffers(1, &idVBO_normals);
        if (idVAO)
            glDeleteVertexArrays(1, &idVAO);
        if (idTexture)
            glDeleteTextures(1, &idTexture);
    }

    void Draw(CProgram &prog)
    {
        // textury
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, idTexture);
        prog.SetInt("tex0", 0);

        glBindVertexArray(idVAO);
        glDrawArrays(GL_TRIANGLES, 0, vertexCount);
        glBindVertexArray(0);
    }

    void SetPosition(const glm::vec3 &pos)
    {
        matModel = glm::translate(glm::mat4(1.0f), pos);
    }

    void SetRotation(const glm::vec3 &angles)
    {
        matModel = glm::rotate(matModel, glm::radians(angles.x), glm::vec3(1, 0, 0));
        matModel = glm::rotate(matModel, glm::radians(angles.y), glm::vec3(0, 1, 0));
        matModel = glm::rotate(matModel, glm::radians(angles.z), glm::vec3(0, 0, 1));
    }

    glm::mat4 GetModelMatrix() const { return matModel; }

private:
    GLuint idVAO, idVBO_coords, idVBO_uvs, idVBO_normals;
    GLuint idTexture;
    size_t vertexCount;
    glm::mat4 matModel;
};

// =======================================================
// KAMERA
// =======================================================
class Camera
{
public:
    Camera() {}

    void Update()
    {
        matView = glm::mat4(1.0f);
        matView = glm::translate(matView, glm::vec3(CameraTranslate_x, CameraTranslate_y, CameraTranslate_z));
        matView = glm::rotate(matView, CameraRotate_x, glm::vec3(1, 0, 0));
        matView = glm::rotate(matView, CameraRotate_y, glm::vec3(0, 1, 0));

        if (windowHeight != 0)
            matProj = glm::perspective(glm::radians(FOV), windowWidth / (float)windowHeight, zNear, zFar);
    }

    void SetParams(float fov, float zn, float zf)
    {
        FOV = fov;
        zNear = zn;
        zFar = zf;
    }

private:
    float FOV = 80.0f;
    float zNear = 0.1f;
    float zFar = 50.0f;
};

// =======================================================
// Globalne zmienne sceny
// =======================================================
CProgram glownyProgram;
CMesh cube3D;
Camera camera;
std::vector<glm::mat4> modelMatrices;

// =======================================================
// INIT
// =======================================================
void Initialize()
{
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0, 0, 0, 1.0f);

    glownyProgram.Init();
    glownyProgram.LoadShaders("vertex.glsl", "fragment.glsl");

    cube3D.CreateFromOBJ("mojmesh.obj");
    cube3D.SetPosition(glm::vec3(0.0, 0.0, 0.0));
    cube3D.LoadTexture("flower32bit.png");

    // ustawiamy kilka różnych macierzy modelu dla kopii
    for (int i = 0; i < 10; ++i)
    {
        for (int j = 0; j < 10; ++j)
        {
            // glm::mat4 mat = glm::translate(glm::mat4(1.0f), glm::vec3((i % 12) * 2.0f - 4.0f, 0.0f, (i / 5) * 2.0f - 2.0f));
            glm::mat4 mat = glm::translate(glm::mat4(1.0f), glm::vec3(i * 1.0f, 0.0f, j * 1.0f));

            // mat = glm::rotate(mat, glm::radians(float(i * 17)), glm::vec3(0, 1, 0));

            float scale = 0.25f + ((j + i) % 3) * 0.25f;
            mat = glm::scale(mat, glm::vec3(scale));

            modelMatrices.push_back(mat);
        }
    }

    camera.SetParams(80.0f, 0.1f, 50.0f);
    camera.Update();
}

// =======================================================
// DISPLAY SCENE
// =======================================================
void DisplayScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    camera.Update();

    glownyProgram.Use();

    glownyProgram.SetMat4("matProj", matProj);
    glownyProgram.SetMat4("matView", matView);
    glownyProgram.SetMat4("matModel", cube3D.GetModelMatrix());

    for (auto &modelMat : modelMatrices)
    {
        glownyProgram.SetMat4("matModel", modelMat);
        cube3D.Draw(glownyProgram);
    }

    cube3D.Draw(glownyProgram);

    glownyProgram.UnUse();
}

// =======================================================
// MAIN
// =======================================================
int main()
{
    if (!glfwInit())
    {
        std::cerr << "GLFW error xdddd!" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(800, 600, "lab05zad02", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "okno error xd!" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "GLAD error xdd!" << std::endl;
        return -1;
    }

    glfwSwapInterval(1); // vsync

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);

    Initialize();

    while (!glfwWindowShouldClose(window))
    {
        DisplayScene();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
