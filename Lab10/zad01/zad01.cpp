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

inline const char *GLErrorToString(GLenum err)
{
    switch (err)
    {
    case GL_NO_ERROR:
        return "GL_NO_ERROR";
    case GL_INVALID_ENUM:
        return "GL_INVALID_ENUM";
    case GL_INVALID_VALUE:
        return "GL_INVALID_VALUE";
    case GL_INVALID_OPERATION:
        return "GL_INVALID_OPERATION";
    case GL_STACK_OVERFLOW:
        return "GL_STACK_OVERFLOW";
    case GL_STACK_UNDERFLOW:
        return "GL_STACK_UNDERFLOW";
    case GL_OUT_OF_MEMORY:
        return "GL_OUT_OF_MEMORY";
    case GL_INVALID_FRAMEBUFFER_OPERATION:
        return "GL_INVALID_FRAMEBUFFER_OPERATION";
    default:
        return "UNKNOWN_ERROR";
    }
}

#define __CHECK_FOR_ERRORS                                                 \
    {                                                                      \
        GLenum errCode;                                                    \
        while ((errCode = glGetError()) != GL_NO_ERROR)                    \
        {                                                                  \
            printf("OpenGL Error %d (%s): in file %s at line %d\n",        \
                   errCode, GLErrorToString(errCode), __FILE__, __LINE__); \
        }                                                                  \
    }

// SCREEN SIZE
int SCREEN_WIDTH = 800;
int SCREEN_HEIGHT = 600;

// GŁOWNA KAMERA
glm::mat4 matProj,
    matView;

// Zmienna czas do animacji
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float time = 0.0f;
float Time = 0.0;

#include "objloader.hpp"
#include "myImGui.hpp"

#include "LightParam.hpp"
#include "MaterialsParam.hpp"

#include "utilities.hpp"

#include "CProgram.hpp"
#include "CMesh.hpp"

CProgram glownyProgram;
CMesh plane;
CMesh monkey;
CMesh tower;
CMesh lightSphere;

#include "minimapa.hpp"
#include "postprocessing.hpp"
#include "shadowMapping.hpp"

#include "CShadowPointLight.hpp"

CShadowPointLight shadowPointLights[MAX_LIGHTS];

#include "shadowPointLight.hpp"

// =======================================================
// INIT
// =======================================================
void Initialize()
{
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0, 0, 0, 1.0f);

    glownyProgram.Init();
    glownyProgram.LoadShaders("shaders/main/vertex.glsl", "shaders/main/fragment.glsl");

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

    // INICJALIZACJA POSTPROCESSINGU
    InitPostProcess();

    // SHADOW MAP
    InitShadowMap();

    for (int i = 0; i < MAX_LIGHTS; i++)
    {
        shadowPointLights[i].Init(lights[i].Position);
    }

    // ShadowPointLight.Init(lights[0].Position);
}

// =======================================================
// DISPLAY SCENE
// =======================================================
void DisplayScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Time += 0.01;
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // KAMERA GŁÓWNA I KAMERA DLA SHADOW MAPY
    UpdateOrbitCamera();
    DirectionalLightCamera();

    // render shadowmap
    if (showShadows)
    {
        RenderShadowMap();
    }

    // rendering pozaekranowy shadowmapboxa
    if (isShadowPointMapping)
    {
        for (int i = 0; i < activeLights; i++)
        {
            shadowPointLights[i].lightPosition = lights[i].Position;
            shadowPointLights[i].UpdateViewMat();

            RenderScene_to_ShadowCubeMap(shadowPointLights[i]);
        }

        __CHECK_FOR_ERRORS
    }

    glownyProgram.Use();
    // glownyProgram.SetInt("tex_shadowCubeMap", 1);

    int baseSlot = 1;

    for (int i = 0; i < MAX_LIGHTS; i++)
    {
        std::string name = "tex_shadowCubeMap[" + std::to_string(i) + "]";
        glownyProgram.SetInt(name.c_str(), baseSlot + i);
    }

    __CHECK_FOR_ERRORS
    glownyProgram.SetInt("showShadows", showShadows ? 1 : 0);
    glownyProgram.SetInt("isShadowPointMapping", isShadowPointMapping ? 1 : 0); // 1 jak maja byc cienie, 0 jak nie
    glownyProgram.SetCameraUniform();
    glownyProgram.SetLightingUniforms(); // właczanie wylaczanie swiatla 1
    glownyProgram.SetFloat("minBiasShadow", minBiasShadow);
    glownyProgram.SetFloat("maxBiasShadow", maxBiasShadow);

    // ANIMACJA PIERWSZEGO ŚWIATŁA PUNKTOWEGO

    if (animateLight)
    {
        lightAngle += 0.1f * deltaTime;

        float radius = 6.0f;

        lights[0].Position.x = radius * cos(lightAngle);
        lights[0].Position.z = radius * sin(lightAngle);
        lights[0].Position.y = 6.0f;
    }

    // WIELE ŚWIATEŁ PUNKTOWYCH

    glownyProgram.SetInt("lightMode", lightMode);
    glownyProgram.SetInt("activeLights", activeLights);

    for (int i = 0; i < activeLights; i++)
    {
        std::string base = "lights[" + std::to_string(i) + "].";

        glownyProgram.sendLightParameters(base, lights[i]);
    }

    glownyProgram.SetShadingProgram(); // imgui shading model wybieranie

    if (showShadows)
    {
        RenderShadowMapOnScreen();
    }

    if (isShadowPointMapping)
    {
        RenderScene_on_Screen();
    }

    // OBIEKTY NA SCENIE
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

    // SFERY W MIEJSCACH ŚWIATEŁ PUNKTOWYCH

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

    // render minimapy do FBO i wyświetlenie minimapy na ekranie
    RenderMiniMap();
    DisplayMiniMapOverlay();

    // render postprocessing
    RenderSceneToFBO();
    RenderPostProcess();
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

    GLFWwindow *window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "lab10zad01", nullptr, nullptr);
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
    for (int i = 0; i < activeLights; i++)
    {
        shadowPointLights[i].Clean();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
