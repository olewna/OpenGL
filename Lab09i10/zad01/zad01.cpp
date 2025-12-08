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
// Zmienna czas do animacji
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float time = 0.0f;
float Time = 0.0;
// Struktura oswietlenia punktowego
struct LightParam
{
    glm::vec3 Ambient;
    glm::vec3 Diffuse;
    glm::vec3 Specular;
    glm::vec3 Attenuation;
    glm::vec3 Position; // Direction dla kierunkowego
    glm::vec3 Direction;
};
// Struktura materialu obiektu
struct MaterialParam
{
    glm::vec3 Ambient;
    glm::vec3 Diffuse;
    glm::vec3 Specular;
    float Shininess;
};

bool showMiniMap = true; // kontrola przez ImGui

#include "objloader.hpp"
#include "myImGui.hpp"
#include "utilities.hpp"

// ----------------------------------------------
// (1) SWIATLO
// ----------------------------------------------

// Przykladowe swiatlo punktowe
LightParam myLight1 = {
    glm::vec3(0.1, 0.1, 0.1),    // ambient
    glm::vec3(1.0, 1.0, 1.0),    // diffuse
    glm::vec3(1.0, 1.0, 1.0),    // specular
    glm::vec3(1.0, 0.0, 0.0),    // attenuation
    glm::vec3(0.0, 6.0, 0.0),    // position
    glm::vec3(0.0f, -1.0f, 0.0f) // direction
};
LightParam myLight2 = {
    glm::vec3(0.1, 0.1, 0.1),    // ambient
    glm::vec3(1.0, 0.0, 0.0),    // diffuse
    glm::vec3(1.0, 1.0, 1.0),    // specular
    glm::vec3(1.0, 0.0, 0.0),    // attenuation
    glm::vec3(2.0, 6.0, 2.0),    // position
    glm::vec3(0.0f, -1.0f, 0.0f) // direction
};
LightParam myLight3 = {
    glm::vec3(0.1, 0.1, 0.1),    // ambient
    glm::vec3(0.0, 1.0, 0.0),    // diffuse
    glm::vec3(1.0, 1.0, 1.0),    // specular
    glm::vec3(1.0, 0.0, 0.0),    // attenuation
    glm::vec3(2.0, 6.0, -2.0),   // position
    glm::vec3(0.0f, -1.0f, 0.0f) // direction
};
LightParam myLight4 = {
    glm::vec3(0.1, 0.1, 0.1),    // ambient
    glm::vec3(0.0, 0.0, 1.0),    // diffuse
    glm::vec3(1.0, 1.0, 1.0),    // specular
    glm::vec3(1.0, 0.0, 0.0),    // attenuation
    glm::vec3(-2.0, 6.0, 2.0),   // position
    glm::vec3(0.0f, -1.0f, 0.0f) // direction
};
LightParam myLight5 = {
    glm::vec3(0.1, 0.1, 0.1),    // ambient
    glm::vec3(1.0, 0.0, 1.0),    // diffuse
    glm::vec3(1.0, 1.0, 1.0),    // specular
    glm::vec3(1.0, 0.0, 0.0),    // attenuation
    glm::vec3(-2.0, 6.0, -2.0),  // position
    glm::vec3(0.0f, -1.0f, 0.0f) // direction
};

const int MAX_LIGHTS = 5;
int activeLights = 2;

LightParam lights[MAX_LIGHTS] = {myLight1, myLight2, myLight3, myLight4, myLight5};
// ----------------------------------------------
// (2) MATERIAL
// ----------------------------------------------

// Przykladowy material
MaterialParam myMaterialMatowy = {
    glm::vec3(0.2, 0.2, 0.2), // ambient
    glm::vec3(1.0, 1.0, 1.0), // diffuse
    glm::vec3(1.0, 1.0, 1.0), // specular
    1.0                       // shininess
};
MaterialParam myMaterialBlysk = {
    glm::vec3(0.2, 0.2, 0.2), // ambient
    glm::vec3(1.0, 1.0, 1.0), // diffuse
    glm::vec3(1.0, 1.0, 1.0), // specular
    256.0                     // shininess
};

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

    void SetShadingProgram()
    {
        glUniform1i(glGetUniformLocation(idProgram, "shadingModel"), shadingModel);
    }

    // Przeslanie parametrow oswietlenia do shadera
    void sendLightParameters(const std::string &name, LightParam light)
    {
        // pobranie id aktualnego programu
        GLint programId = idProgram;
        glGetIntegerv(GL_CURRENT_PROGRAM, &programId);

        glUniform3fv(glGetUniformLocation(programId, (name + "Ambient").c_str()), 1, glm::value_ptr(light.Ambient));
        glUniform3fv(glGetUniformLocation(programId, (name + "Diffuse").c_str()), 1, glm::value_ptr(light.Diffuse));
        glUniform3fv(glGetUniformLocation(programId, (name + "Specular").c_str()), 1, glm::value_ptr(light.Specular));
        glUniform3fv(glGetUniformLocation(programId, (name + "Attenuation").c_str()), 1, glm::value_ptr(light.Attenuation));
        glUniform3fv(glGetUniformLocation(programId, (name + "Position").c_str()), 1, glm::value_ptr(light.Position));
        glUniform3fv(glGetUniformLocation(programId, (name + "Direction").c_str()), 1, glm::value_ptr(light.Direction));
    }

    // Przeslanie parametrow materialow do shadera
    void sendMaterialParameters(MaterialParam material)
    {
        // pobranie id aktualnego programu
        GLint programId = idProgram;
        glGetIntegerv(GL_CURRENT_PROGRAM, &programId);

        glUniform3fv(glGetUniformLocation(programId, "myMaterial.Ambient"), 1, glm::value_ptr(material.Ambient));
        glUniform3fv(glGetUniformLocation(programId, "myMaterial.Diffuse"), 1, glm::value_ptr(material.Diffuse));
        glUniform3fv(glGetUniformLocation(programId, "myMaterial.Specular"), 1, glm::value_ptr(material.Specular));
        glUniform1f(glGetUniformLocation(programId, "myMaterial.Shininess"), material.Shininess);
    }

    void SetLightingUniforms()
    {
        GLint loc = glGetUniformLocation(idProgram, "uLightingEnabled");
        if (loc != -1)
            glUniform1i(loc, lightingEnabled ? 1 : 0);
    }

    void SetCameraUniform()
    {
        glm::vec3 cameraPos = ExtractCameraPos(matView);

        glUniform3fv(glGetUniformLocation(idProgram, "cameraPos"), 1, glm::value_ptr(cameraPos));
    }

    void SetDrawLightSphere(bool v)
    {
        glUniform1i(glGetUniformLocation(idProgram, "uDrawLightSphere"), v ? 1 : 0);
    }

    GLuint GetId() const
    {
        return idProgram;
    }

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

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

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
        // printf("internalFormat = 0x%X\n", internalFormat);
        // printf("format = 0x%X\n", format);

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

CProgram glownyProgram;
CMesh plane;
CMesh monkey;
CMesh tower;
CMesh lightSphere;

// =======================================================
// MINIMAPA
// =======================================================
GLuint fboMiniMap, texMiniMap, rboMiniMap;
GLuint vaoMiniMap, vboMiniMap;
int minimapSize = 500;
CProgram minimapProgram;

void InitializeMiniMap()
{
    // Tworzymy FBO
    glGenFramebuffers(1, &fboMiniMap);
    glBindFramebuffer(GL_FRAMEBUFFER, fboMiniMap);

    // Tworzymy teksturę
    glGenTextures(1, &texMiniMap);
    glBindTexture(GL_TEXTURE_2D, texMiniMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, minimapSize, minimapSize, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texMiniMap, 0);

    // Tworzymy renderbuffer depth
    glGenRenderbuffers(1, &rboMiniMap);
    glBindRenderbuffer(GL_RENDERBUFFER, rboMiniMap);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, minimapSize, minimapSize);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboMiniMap);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "ERROR: MiniMap FBO incomplete!\n";

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    float quadVertices[] = {
        // x, y,            u, v
        0.6f, 0.6f, 0.0f, 0.0f, // lewy dolny
        1.0f, 0.6f, 1.0f, 0.0f, // prawy dolny
        1.0f, 1.0f, 1.0f, 1.0f, // prawy górny

        0.6f, 0.6f, 0.0f, 0.0f, // lewy dolny
        1.0f, 1.0f, 1.0f, 1.0f, // prawy górny
        0.6f, 1.0f, 0.0f, 1.0f  // lewy górny
    };

    glGenVertexArrays(1, &vaoMiniMap);
    glBindVertexArray(vaoMiniMap);
    glGenBuffers(1, &vboMiniMap);
    glBindBuffer(GL_ARRAY_BUFFER, vboMiniMap);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    glBindVertexArray(0);

    minimapProgram.Init();
    minimapProgram.LoadShaders("minimap_vertex.glsl", "minimap_fragment.glsl");
}

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

    plane.CreateFromOBJ("ground-large.obj");
    plane.LoadTexture("grass.jpg");

    tower.CreateFromOBJ("tower.obj");
    tower.SetPosition(glm::vec3(5.0, -1.3, 0.0));
    tower.LoadTexture("wood.jpg");

    monkey.CreateFromOBJ("monkey.obj");
    monkey.LoadTexture("brick.jpg");

    lightSphere.CreateFromOBJ("sphere.obj");
    UpdateOrbitCamera();

    // INICJALIZACJA MINIMAPY
    InitializeMiniMap();
}

// =======================================================
// RENDER MINIMAPY
// =======================================================
void RenderMiniMap()
{
    if (!showMiniMap)
        return;

    // ustawienie FBO
    glViewport(0, 0, minimapSize, minimapSize);
    glBindFramebuffer(GL_FRAMEBUFFER, fboMiniMap);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // widok z góry
    glownyProgram.Use();
    glm::vec3 camPos = ExtractCameraPos(matView);
    glm::mat4 miniView = glm::lookAt(glm::vec3(camPos.x, 10.0f, camPos.z), glm::vec3(camPos.x, 0.0f, camPos.z), glm::vec3(0.0f, 0.0f, -1.0f));
    glm::mat4 miniProj = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 50.0f);

    glownyProgram.SetMat4("matView", miniView);
    glownyProgram.SetMat4("matProj", miniProj);

    // render sceny
    glownyProgram.SetMat4("matModel", plane.GetModelMatrix());
    glownyProgram.sendMaterialParameters(myMaterialMatowy);
    plane.Draw(glownyProgram);

    glownyProgram.SetMat4("matModel", tower.GetModelMatrix());
    tower.Draw(glownyProgram);

    glownyProgram.SetMat4("matModel", monkey.GetModelMatrix());
    glownyProgram.sendMaterialParameters(myMaterialBlysk);
    monkey.Draw(glownyProgram);

    glownyProgram.UnUse();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // przywrócenie viewportu głównego okna
    int width, height;
    glfwGetFramebufferSize(glfwGetCurrentContext(), &width, &height);
    glViewport(0, 0, width, height);
}

// =======================================================
// WYŚWIETLANIE MINIMAPY NA EKRANIE
// =======================================================
void DisplayMiniMapOverlay()
{
    if (!showMiniMap)
        return;

    glDisable(GL_DEPTH_TEST);
    // glUseProgram(0);

    minimapProgram.Use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texMiniMap);
    minimapProgram.SetInt("tex0", 0);

    glBindVertexArray(vaoMiniMap);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
}

// =======================================================
// DISPLAY SCENE
// =======================================================
void DisplayScene()
{
    // if (show_animation)
    Time += 0.01;
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // camera.Update();
    UpdateOrbitCamera();

    glownyProgram.Use();

    glownyProgram.SetCameraUniform();

    glownyProgram.SetLightingUniforms(); // właczanie wylaczanie swiatla 1

    if (animateLight)
    {
        lightAngle += 0.1f * deltaTime;

        float radius = 6.0f;

        lights[0].Position.x = radius * cos(lightAngle);
        lights[0].Position.z = radius * sin(lightAngle);
        lights[0].Position.y = 6.0f;
    }
    // else
    // {
    //     myLight.Position.x = 0.0f;
    //     myLight.Position.z = 0.0f;
    //     myLight.Position.y = 6.0f;
    // }

    glownyProgram.SetInt("lightMode", lightMode);

    glownyProgram.SetInt("activeLights", activeLights);

    for (int i = 0; i < activeLights; i++)
    {
        std::string base = "lights[" + std::to_string(i) + "].";

        glownyProgram.sendLightParameters(base, lights[i]);
    }

    glownyProgram.SetShadingProgram(); // imgui shading model wybieranie

    time += deltaTime;

    float angleY = time;
    float angleX = 45.0f * time;

    glm::vec3 initialPosition(2.0f, 6.0f, 0.0f);
    glm::vec3 newPosition;
    newPosition.x = initialPosition.x * cos(angleY) - initialPosition.z * sin(angleY);
    newPosition.z = initialPosition.x * sin(angleY) + initialPosition.z * cos(angleY);
    newPosition.y = initialPosition.y;

    monkey.SetPosition(newPosition);
    monkey.SetRotation(glm::vec3(angleX, angleY, 0.0f));

    glownyProgram.SetMat4("matProj", matProj);
    glownyProgram.SetMat4("matView", matView);

    glownyProgram.SetMat4("matModel", plane.GetModelMatrix());
    glownyProgram.sendMaterialParameters(myMaterialMatowy);
    plane.Draw(glownyProgram);

    glownyProgram.SetMat4("matModel", tower.GetModelMatrix());
    tower.Draw(glownyProgram);

    glownyProgram.SetMat4("matModel", monkey.GetModelMatrix());
    glownyProgram.sendMaterialParameters(myMaterialBlysk);
    monkey.Draw(glownyProgram);

    if (lightMode == 0)
    {
        float sphereScale = 0.1f;
        glownyProgram.SetDrawLightSphere(true);

        for (int i = 0; i < activeLights; i++)
        {
            glm::mat4 matLight = glm::mat4(1.0f);
            matLight = glm::translate(matLight, lights[i].Position);
            matLight = glm::scale(matLight, glm::vec3(sphereScale));
            glownyProgram.SetInt("lightIndexToDraw", i);

            glownyProgram.SetMat4("matModel", matLight);
            lightSphere.Draw(glownyProgram);
        }
        glownyProgram.SetDrawLightSphere(false);
    }

    // glownyProgram.UnUse();

    // render minimapy do FBO
    RenderMiniMap();

    // wyświetlenie minimapy na ekranie
    DisplayMiniMapOverlay();
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

    // IMGUI
    float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor());

    GLFWwindow *window = glfwCreateWindow(800, 600, "lab06i07zad01", nullptr, nullptr);
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

    framebuffer_size_callback(window, 800, 600);

    // IMGUI
    ImGui_Init(window);

    Initialize();

    while (!glfwWindowShouldClose(window))
    {
        DisplayScene();

        // IMGUI
        ImGui_Display();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // IMGUI
    ImGui_Clean();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
