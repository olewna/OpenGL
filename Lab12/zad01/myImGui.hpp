// ---------------------------------------------------
// Obsluga ImGui
// ---------------------------------------------------
#ifndef __MY_IMGUI_HPP
#define __MY_IMGUI_HPP

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

// -----------------------------------
// ImGui ImGui ImGui ImGui ImGui ImGui
// -----------------------------------

#include "LightParam.hpp"
#include "utilities.hpp"

bool lightingEnabled = true;
bool animateLight = false;
float lightAngle = 0.0f;
int shadingModel = 0;
int lightMode = 0; // 0 - punktowe 1 kierunkunowe

bool showMiniMap = false; // kontrola minimapy

int currentEffect = 0;           // postprocessing efekt
bool showPostprocessing = false; // kontrola postprocessingu

bool showShadows = false;

float minBiasShadow = 0.001f;
float maxBiasShadow = 0.005f;

bool isShadowPointMapping = false;

bool animationMonkey = true;
bool useEnvMapping = false;

int score = 0;
const int CEL_GRY = 1;
bool gameWon = false;

// Naglowki funkcji
void ImGui_Init(GLFWwindow *window);
void ImGui_Display();
void ImGui_Clean();

// -----------------------------------
// ImGui ImGui ImGui ImGui ImGui ImGui
// -----------------------------------
void ImGui_Init(GLFWwindow *window)
{
    const char *glsl_version = "#version 330";
    float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor());

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   // Enable Multi-Viewport / Platform Windows
    // io.ConfigViewportsNoAutoMerge = true;
    // io.ConfigViewportsNoTaskBarIcon = true;
    //  Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsLight();
    //  Setup scaling
    ImGuiStyle &style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale); // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = main_scale; // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)
#if GLFW_VERSION_MAJOR >= 3 && GLFW_VERSION_MINOR >= 3
    io.ConfigDpiScaleFonts = true;     // [Experimental] Automatically overwrite style.FontScaleDpi in Begin() when Monitor DPI changes. This will scale fonts but _NOT_ scale sizes/padding for now.
    io.ConfigDpiScaleViewports = true; // [Experimental] Scale Dear ImGui and Platform Windows when Monitor DPI changes.
#endif

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
#ifdef __EMSCRIPTEN__
    ImGui_ImplGlfw_InstallEmscriptenCallbacks(window, "#canvas");
#endif
    ImGui_ImplOpenGL3_Init(glsl_version);
}

// -----------------------------------
// ImGui ImGui ImGui ImGui ImGui ImGui
// -----------------------------------
void ImGui_Display()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGuiIO &io = ImGui::GetIO();

    /// -----------------------------------
    /// OKNO IMGUI NAJWAŻNIEJSZE!!!!!!
    /// -----------------------------------
    {
        static float f = 0.0f;
        static int counter = 0;

        glm::vec3 camPos = ExtractCameraPos(matView);

        // RODZAJ CIENIOWANIA
        const char *shadingItems[] = {"Phong", "Blinn-Phong"};
        ImGui::Begin("Lighting lab06&07 opengl glfw");
        ImGui::Combo("Shading Model", &shadingModel, shadingItems, IM_ARRAYSIZE(shadingItems));

        // WŁACZANIE SWIATLA I ANIMACJI
        ImGui::Checkbox("Lighting", &lightingEnabled);
        ImGui::Checkbox("Lighting animation", &animateLight);

        // TYP ŚWIATŁA NUMER 1
        ImGui::Text("Light type:");
        ImGui::RadioButton("Point", &lightMode, 0);
        ImGui::RadioButton("Directional", &lightMode, 1);

        // ŚWIATŁA
        ImGui::SliderInt("Amount of lights:", &activeLights, 1, MAX_LIGHTS);
        for (int i = 0; i < activeLights; i++)
        {
            if (lightMode == 1 && i == 0)
            {
                ImGui::ColorEdit3("Ambient", glm::value_ptr(lights[i].Ambient));
                ImGui::ColorEdit3("Diffuse", glm::value_ptr(lights[i].Diffuse));
                ImGui::ColorEdit3("Specular", glm::value_ptr(lights[i].Specular));
                ImGui::DragFloat3("Position", glm::value_ptr(lights[i].Position), 0.1f);
                ImGui::DragFloat3("Direction", glm::value_ptr(lights[i].Direction), 0.1f);
            }
            else if (lightMode == 0)
            {
                std::string name = "Light# " + std::to_string(i + 1);
                if (ImGui::TreeNode(name.c_str()))
                {
                    ImGui::ColorEdit3("Ambient", glm::value_ptr(lights[i].Ambient));
                    ImGui::ColorEdit3("Diffuse", glm::value_ptr(lights[i].Diffuse));
                    ImGui::ColorEdit3("Specular", glm::value_ptr(lights[i].Specular));
                    ImGui::DragFloat3("Attenuation", glm::value_ptr(lights[i].Attenuation), 0.01f);
                    ImGui::DragFloat3("Position", glm::value_ptr(lights[i].Position), 0.1f);

                    ImGui::TreePop();
                }
            }
        }

        // POKAŻ MINIMAPE
        ImGui::Checkbox("Show minimap", &showMiniMap);

        // WYBIERZ RODZAJ POSTPROCESSINGU
        ImGui::Text("Postprocessing type:");
        ImGui::RadioButton("NEGATYW", &currentEffect, 0);
        ImGui::RadioButton("SHARPEN", &currentEffect, 1);

        // POKAŻ POSTPROCESSING
        ImGui::Checkbox("Show postprocessing", &showPostprocessing);

        // POKAŻ cienie directional
        ImGui::Checkbox("Show directional shadows", &showShadows);

        // POKAŻ cienie directional
        ImGui::Checkbox("Show point light shadows", &isShadowPointMapping);

        // SHADOW BIAS
        ImGui::DragFloat("MinBias", &minBiasShadow, 0.01f, 0.001f, 1.0f);
        ImGui::DragFloat("MaxBias", &maxBiasShadow, 0.01f, 0.001f, 1.0f);

        // monkey animation
        ImGui::Checkbox("Monkey animation", &animationMonkey);

        // env mapping
        ImGui::Checkbox("Env mapping", &useEnvMapping);

        ImGui::Text("Time: %.1f ", Time);
        ImGui::Text("SCORE: %.1i ", score);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::Text("Camera Pos: %.2f, %.2f, %.2f", camPos.x, camPos.y, camPos.z);
        ImGui::End();
    }

    // Zamykanie procesu renderowania ImGui
    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow *backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}

// -----------------------------------
// ImGui ImGui ImGui ImGui ImGui ImGui
// -----------------------------------
void ImGui_Clean()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

#endif
