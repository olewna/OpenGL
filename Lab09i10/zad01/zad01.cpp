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

// GŁOWNA KAMERA
glm::mat4 matProj, matView;

// Zmienna czas do animacji
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float time = 0.0f;
float Time = 0.0;

bool showMiniMap = true; // kontrola przez ImGui

#include "objloader.hpp"
#include "myImGui.hpp"
#include "utilities.hpp"

#include "LightParam.hpp"
#include "MaterialsParam.hpp"
#include "CProgram.hpp"
#include "CMesh.hpp"

CProgram glownyProgram;
CMesh plane;
CMesh monkey;
CMesh tower;
CMesh lightSphere;

#include "minimapa.hpp"

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

    plane.CreateFromOBJ("objs/ground-large.obj");
    plane.LoadTexture("assets/grass.jpg");

    tower.CreateFromOBJ("objs/tower.obj");
    tower.SetPosition(glm::vec3(5.0, -1.3, 0.0));
    tower.LoadTexture("assets/wood.jpg");

    monkey.CreateFromOBJ("objs/monkey.obj");
    monkey.LoadTexture("assets/brick.jpg");

    lightSphere.CreateFromOBJ("objs/sphere.obj");
    UpdateOrbitCamera();

    // INICJALIZACJA MINIMAPY
    InitializeMiniMap();
}

// =======================================================
// DISPLAY SCENE
// =======================================================
void DisplayScene()
{
    Time += 0.01;
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

    glownyProgram.UnUse();

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
