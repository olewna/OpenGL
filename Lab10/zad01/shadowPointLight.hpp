#ifndef __SHADOW_POINT_LIGHT
#define __SHADOW_POINT_LIGHT

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "utilities.hpp"
#include "CProgram.hpp"
#include "CShadowPointLight.hpp"

void RenderScene_to_ShadowCubeMap(CShadowPointLight &spl)
{
    // -------------------------------------------------------
    // NOWE: Generowanie szesciennej mapy cieni
    // -------------------------------------------------------

    // Uruchomienie programu do generowania mapy cieni
    // Przeslanie wszystkich potrzebnych informacji do shaderow
    // Ukrywamy to wszystko w metodzie GenBegin()
    // To ona ma wiedziec co robic

    // Renderowanie 6 razy do kazdej sciany cube mapy
    // Gdy poznamy shader geometrii, ten etap zostanie zamieniony
    // na jedno przejscie
    for (int face = 0; face < 6; face++)
    {
        __CHECK_FOR_ERRORS
        spl.GenBegin(face);

        // Renderujemy obiekt przy uzyciu aktualnego programu
        // Koniecznie wysylamy macierz modelu, aby potok wyrenderowal obiekt
        // w odpowiednim miejscu sceny
        __CHECK_FOR_ERRORS
        ShadowPointLightProgram.SetMat4("matModel", plane.GetModelMatrix());
        ShadowPointLightProgram.sendMaterialParameters(myMaterialMatowy);
        plane.Draw(ShadowPointLightProgram);
        __CHECK_FOR_ERRORS

        ShadowPointLightProgram.SetMat4("matModel", tower.GetModelMatrix());
        tower.Draw(ShadowPointLightProgram);

        ShadowPointLightProgram.SetMat4("matModel", monkey.GetModelMatrix());
        ShadowPointLightProgram.sendMaterialParameters(myMaterialBlysk);
        monkey.Draw(ShadowPointLightProgram);
        __CHECK_FOR_ERRORS
    }

    // Zakonczenie procesu generowania cieni
    spl.GenEnd();

    int width, height;
    glfwGetFramebufferSize(glfwGetCurrentContext(), &width, &height);
    glViewport(0, 0, width, height);
    matProj = glm::perspective(glm::radians(80.0f), (float)width / (float)height, 0.1f, 50.0f);
}

void RenderScene_on_Screen()
{
    // Przelaczamy sie na domyslny frame buffer
    // glViewport(0, 0, windowWidth, windowHeight);
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glm::vec3 cameraPos = ExtractCameraPos(matView);
    for (int i = 0; i < activeLights; i++)
    {
        shadowPointLights[i].SendUniforms(glownyProgram, 1 + i, cameraPos, i);
    }

    // ShadowPointLight.SendUniforms(glownyProgram, 1, cameraPos);
    // ShadowPointLightProgram.UnUse();

    // WYLACZAMY program
}

#endif