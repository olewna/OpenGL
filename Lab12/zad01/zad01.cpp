#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/random.hpp>

GLFWwindow *window = nullptr;

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
float SCREEN_WIDTH = 800.0f;
float SCREEN_HEIGHT = 600.0f;

// GŁOWNA KAMERA
glm::mat4 matProj, matView;

// Zmienna czas do animacji
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float myTime = 0.0f;
float Time = 0.0;

#include "objloader.hpp"
#include "myImGui.hpp"

#include "LightParam.hpp"
#include "MaterialsParam.hpp"

#include "text-ft.hpp"
#include "utilities.hpp"

#include "CProgram.hpp"
CProgram glownyProgram;
#include "CMesh.hpp"

#include "CGround.hpp"
#include "CPlayer.hpp"

CGround myGround;
CPlayer myPlayer;

// CMesh plane; // ZAMIAST TEGO JEST myGround!!!!!!
CMesh monkey;
CMesh tower;
CMesh lightSphere;
CMesh flower;
CMesh tree;
CMesh collectible;

#include "minimapa.hpp"
#include "postprocessing.hpp"
#include "shadowMapping.hpp"

#include "CShadowPointLight.hpp"

CShadowPointLight shadowPointLights[MAX_LIGHTS];

#include "shadowPointLight.hpp"
#include "skybox.hpp"
#include "environmental_mapping.hpp"

struct RandomObjectInstance
{
    glm::vec3 position;
    float scale;
    float angleY;
    glm::mat4 modelMatrix;
    glm::vec3 colliderPosition;
    float baseY;
    float phase;

    bool isCollectible = false;
    bool collected = false;
};

const int FLOWER_COUNT = 100;
std::vector<RandomObjectInstance> flowerInstances;

const int TREES_COUNT = 50;
std::vector<RandomObjectInstance> treeInstances;

const int COLLECTIBLES_COUNT = 10;
std::vector<RandomObjectInstance> collectibleInstances;

#include "CCollider.hpp"
std::vector<CSphereCollider *> sceneObjects;

float fps = 0.0f;
float fpsTimer = 0.0f;
int fpsFrames = 0;

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

    // plane.CreateFromOBJ("objs/ground-large.obj");
    // plane.LoadTexture("assets/grass.jpg");
    myGround.Init(
        // "objs/ground-large.obj",
        // "objs/scene-plane.obj",
        "objs/scene-levels.obj",
        // "objs/scene-large.obj",
        "assets/grass.jpg");

    tower.CreateFromOBJ("objs/tower.obj");
    tower.LoadTexture("assets/wood.jpg");
    tower.SetCollision(2.0f, "tower");

    sceneObjects.push_back(tower.GetCollision());

    monkey.CreateFromOBJ("objs/monke.obj");
    monkey.LoadTexture("assets/brick.jpg");

    myPlayer.Init(
        &myGround,
        "objs/lego.obj",
        "assets/lego.png");

    // KWIATY
    flower.CreateFromOBJ("objs/flower.obj");
    flower.LoadTexture("assets/flower32bit.png");

    std::srand(static_cast<unsigned>(std::time(nullptr)));
    float minX = -20.0f;
    float maxX = 20.0f;
    float minZ = -20.0f;
    float maxZ = 20.0f;

    __CHECK_FOR_ERRORS
    for (int i = 0; i < FLOWER_COUNT; ++i)
    {
        RandomObjectInstance fi;
        fi.scale = glm::linearRand(0.5f, 1.5f);
        float x = minX + (maxX - minX) * (rand() / (float)RAND_MAX);
        float z = minZ + (maxZ - minZ) * (rand() / (float)RAND_MAX);
        float y = myGround.getHighestY(glm::vec2(x, z)) + fi.scale * -0.5f;
        fi.position = glm::vec3(x, y + 1.0, z);
        fi.angleY = glm::radians(glm::linearRand(0.0f, 360.0f));

        fi.modelMatrix = glm::translate(glm::mat4(1.0f), fi.position);
        fi.modelMatrix = glm::rotate(fi.modelMatrix, fi.angleY, glm::vec3(0, 1, 0));
        fi.modelMatrix = glm::scale(fi.modelMatrix, glm::vec3(fi.scale));

        flowerInstances.push_back(fi);
    }
    __CHECK_FOR_ERRORS

    // DRZEWA
    tree.CreateFromOBJ("objs/tree.obj");
    tree.LoadTexture("assets/leaves.jpg");

    for (int i = 0; i < TREES_COUNT; ++i)
    {
        RandomObjectInstance tr;
        tr.scale = glm::linearRand(0.5f, 1.5f);
        float x = minX + (maxX - minX) * (rand() / (float)RAND_MAX);
        float z = minZ + (maxZ - minZ) * (rand() / (float)RAND_MAX);
        float y = myGround.getHighestY(glm::vec2(x, z)) + tr.scale * -0.5f;
        tr.position = glm::vec3(x, y, z);
        tr.angleY = glm::radians(glm::linearRand(0.0f, 360.0f));

        tr.modelMatrix = glm::translate(glm::mat4(1.0f), tr.position);
        tr.modelMatrix = glm::rotate(tr.modelMatrix, tr.angleY, glm::vec3(0, 1, 0));
        tr.modelMatrix = glm::scale(tr.modelMatrix, glm::vec3(tr.scale));

        treeInstances.push_back(tr);

        float colliderRadius = 0.5f * tr.scale;
        CSphereCollider *collider = new CSphereCollider(tr.position, colliderRadius, "tree" + std::to_string(i));

        sceneObjects.push_back(collider);
    }
    __CHECK_FOR_ERRORS

    // COLLECTIBLES
    collectible.CreateFromOBJ("objs/donut.obj");
    collectible.LoadTexture("assets/brick.jpg");

    for (int i = 0; i < COLLECTIBLES_COUNT; ++i)
    {
        RandomObjectInstance col;
        col.scale = 12.0f;
        float x = minX + (maxX - minX) * (rand() / (float)RAND_MAX);
        float z = minZ + (maxZ - minZ) * (rand() / (float)RAND_MAX);
        float y = myGround.getHighestY(glm::vec2(x, z)) + 1.0f;
        col.position = glm::vec3(x, y, z);
        col.angleY = glm::radians(glm::linearRand(0.0f, 360.0f));

        col.modelMatrix = glm::translate(glm::mat4(1.0f), col.position);
        col.modelMatrix = glm::rotate(col.modelMatrix, col.angleY, glm::vec3(0, 1, 0));
        col.modelMatrix = glm::scale(col.modelMatrix, glm::vec3(col.scale));

        collectibleInstances.push_back(col);
        col.isCollectible = true;

        float colliderRadius = 0.5f;
        col.colliderPosition = col.position;
        col.colliderPosition.y -= 1.0f;
        CSphereCollider *collider = new CSphereCollider(col.colliderPosition, colliderRadius, "collectible_" + std::to_string(i));

        sceneObjects.push_back(collider);
    }

    lightSphere.CreateFromOBJ("objs/sphere.obj");
    UpdateOrbitCamera();
    __CHECK_FOR_ERRORS

    CreateSkyBox();
    __CHECK_FOR_ERRORS

    // INICJALIZACJA MINIMAPY
    InitializeMiniMap();
    __CHECK_FOR_ERRORS

    // INICJALIZACJA POSTPROCESSINGU
    InitPostProcess();
    __CHECK_FOR_ERRORS

    // SHADOW MAP
    InitShadowMap();

    __CHECK_FOR_ERRORS

    // environmental mapping
    InitEnvironmentMap();

    for (int i = 0; i < MAX_LIGHTS; i++)
    {
        shadowPointLights[i].Init(lights[i].Position);
    }
}

// =======================================================
// DISPLAY SCENE
// =======================================================
void DisplayScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Time += 0.01;

    // FPS-y
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    fpsFrames++;
    fpsTimer += deltaTime;

    if (fpsTimer >= 1.0f)
    {
        fps = fpsFrames / fpsTimer;
        fpsFrames = 0;
        fpsTimer = 0.0f;
    }

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

    if (useEnvMapping)
    {
        RenderEnvironmentMap(monkey.GetPosition());
    }

    glownyProgram.Use();

    glownyProgram.SetInt("tex_environment", 10);

    // ustawianie shadowcubemapy na defaultową jakas
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
    if (animationMonkey)
    {
        myTime += deltaTime;
        float angleY = myTime;
        float angleX = 45.0f * myTime;
        glm::vec3 initialPosition(2.0f, 6.0f, 0.0f);
        glm::vec3 newPosition;
        newPosition.x = initialPosition.x * cos(angleY) - initialPosition.z * sin(angleY);
        newPosition.z = initialPosition.x * sin(angleY) + initialPosition.z * cos(angleY);
        newPosition.y = initialPosition.y;

        monkey.SetPosition(newPosition);
        monkey.SetRotation(glm::vec3(angleX, angleY, 0.0f));
    }

    if (useEnvMapping)
    {
        BindSkyboxTexture();
    }

    glownyProgram.SetMat4("matProj", matProj);
    glownyProgram.SetMat4("matView", matView);

    // environmental mapping unifroms
    glownyProgram.SetInt("uUseEnvMap", useEnvMapping ? 1 : 0);
    glownyProgram.SetFloat("envStrength", 0.0f);

    // glownyProgram.SetMat4("matModel", plane.GetModelMatrix());
    glownyProgram.sendMaterialParameters(myMaterialMatowy);
    // plane.Draw(glownyProgram);
    myGround.Draw(glownyProgram); // MAPA

    float x = 10.0f;
    float z = 0.0f;
    float y = myGround.getHighestY(glm::vec2(x, z));
    // std::cout << "Y = " << y << std::endl;

    if (!std::isnan(y))
    {
        tower.SetPosition(glm::vec3(x, y - 0.6f, z));
    }

    // if (tower.GetCollision())
    //     tower.GetCollision()->Position = tower.GetPosition();

    glownyProgram.SetMat4("matModel", tower.GetModelMatrix());
    tower.Draw(glownyProgram);

    // environmental mapping unifroms
    glownyProgram.SetInt("uUseEnvMap", useEnvMapping ? 1 : 0);
    glownyProgram.SetFloat("envStrength", 0.8f);

    glownyProgram.SetMat4("matModel", monkey.GetModelMatrix());
    glownyProgram.sendMaterialParameters(myMaterialBlysk);
    monkey.Draw(glownyProgram);
    glownyProgram.SetFloat("envStrength", 0.0f);

    glownyProgram.sendMaterialParameters(myMaterialMatowy);
    // player
    //  myPlayer.SetPosition(glm::vec3(0.0, y, 0.0));
    myPlayer.Draw(glownyProgram); // GRACZ

    // KWIATY
    for (const auto &fi : flowerInstances)
    {
        glownyProgram.SetMat4("matModel", fi.modelMatrix);
        flower.Draw(glownyProgram);
    }

    // DRZEWA
    for (const auto &tr : treeInstances)
    {
        glownyProgram.SetMat4("matModel", tr.modelMatrix);
        tree.Draw(glownyProgram);
    }

    // COLLECTIBLES
    // for (int i = 0; i < collectibleInstances.size(); ++i)
    // {
    //     CSphereCollider *colCollider =
    //         sceneObjects[1 + FLOWER_COUNT + TREES_COUNT + i];

    //     if (!colCollider->active)
    //         continue;

    //     glownyProgram.SetMat4("matModel", collectibleInstances[i].modelMatrix);
    //     collectible.Draw(glownyProgram);
    // }

    for (auto &col : collectibleInstances)
    {
        // ./if (col.collected) continue;

        // float hover = sin(currentFrame + col.phase) * 0.3f;
        // glm::vec3 pos = col.position;
        // pos.y = col.baseY + hover;
        // col.modelMatrix = glm::translate(col.modelMatrix, pos);

        glownyProgram.SetMat4("matModel", col.modelMatrix);
        collectible.Draw(glownyProgram);
    }

    // environmental mapping unifroms
    glownyProgram.SetInt("uUseEnvMap", useEnvMapping ? 1 : 0);
    glownyProgram.SetFloat("envStrength", 0.0f);

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

    // skybox
    DrawSkyBox(matView, matProj);

    // render postprocessing
    RenderSceneToFBO();
    RenderPostProcess();

    // render minimapy do FBO i wyświetlenie minimapy na ekranie
    RenderMiniMap();
    DisplayMiniMapOverlay();

    int w, h;
    glfwGetFramebufferSize(window, &w, &h);

    std::string fpsText = "FPS: " + std::to_string((int)fps);

    RenderText(
        fpsText,
        10.0f, // X ->
        10.0f, // Y  V
        2.0f,
        glm::vec3(1.0f, 0.0f, 0.0f)); // rgb kolor napisu
}

void keyboard_handler()
{
    float speed = 0.1; // a moze uzaleznic od FPS?
    float rotate = 0.1f;

    if (__keys[GLFW_KEY_W])
        myPlayer.Move(speed, sceneObjects);

    if (__keys[GLFW_KEY_S])
        myPlayer.Move(-speed, sceneObjects);

    if (__keys[GLFW_KEY_D])
        myPlayer.Rotate(-rotate);

    if (__keys[GLFW_KEY_A])
        myPlayer.Rotate(rotate);
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

    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "lab12", nullptr, nullptr);
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

    InitText("assets/arial.ttf", 24);

    while (!glfwWindowShouldClose(window))
    {
        keyboard_handler();

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
