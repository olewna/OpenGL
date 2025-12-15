#ifndef __CSHADOW_POINT_LIGHT
#define __CSHADOW_POINT_LIGHT

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

CProgram ShadowPointLightProgram;

// ---------------------------------------
class CShadowPointLight
{

public:
    // Polozenie swiatla
    glm::vec3 lightPosition;

    // Identyfikatory obiektow
    GLuint idProgram;        // program generujacy mape cieni
    GLuint idTextureCube;    // tekstura
    GLuint idFrameBuffer[6]; // 6 x FBO

    // Rozmiar mapy cieni
    int DepthMap_Width = 1024;
    int DepthMap_Height = 1024;

    // Bryla obcinania (frustum)
    float frustumNear = 1.0f;
    float frustumFar = 20.0f;
    // macierz projection oraz view
    // do renderingu kazdej z szesciu scian
    glm::mat4 matProj;
    glm::mat4 matViews[6];

    // Aktualizacja polozenia oswietlenia i macierzy rzutowania
    // potrzebna podczas inicjalizacji oraz zawsze wtedy, kiedy
    // dokonujemy zmiany polozenia oswietlenia
    void UpdateViewMat()
    {
        // Obliczanie nowych macierzy widoku
        matViews[0] = glm::lookAt(lightPosition, lightPosition + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
        matViews[1] = glm::lookAt(lightPosition, lightPosition + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
        matViews[2] = glm::lookAt(lightPosition, lightPosition + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0));
        matViews[3] = glm::lookAt(lightPosition, lightPosition + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0));
        matViews[4] = glm::lookAt(lightPosition, lightPosition + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0));
        matViews[5] = glm::lookAt(lightPosition, lightPosition + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0));
    }

    // ----------------------------------------------------------
    // Etap 1. Inicjalizacja calego procesu generowania
    // cieni z podaniem punktu polozenia zrodla swiatla
    void Init(glm::vec3 _pos)
    {
        // 1. Pozycja zrodla swiatla
        this->lightPosition = _pos;

        // 2. Textura do depthMapy
        glGenTextures(1, &idTextureCube);
        glBindTexture(GL_TEXTURE_CUBE_MAP, idTextureCube);
        assert(idTextureCube != 0);

        for (int i = 0; i < 6; ++i)
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, DepthMap_Width, DepthMap_Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        // 3. 6 x FBO z buforem glebokosci na kazda sciane
        for (int i = 0; i < 6; i++)
        {
            glGenFramebuffers(1, &idFrameBuffer[i]);
            glBindFramebuffer(GL_FRAMEBUFFER, idFrameBuffer[i]);
            // glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, idTextureCube, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, idTextureCube, 0);
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            {
                printf("Error: Framebuffer %d is not complete!\n", i);
                exit(1);
            }
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 4. Potok do generowania samych cieni
        ShadowPointLightProgram.Init();
        idProgram = ShadowPointLightProgram.GetId();
        ShadowPointLightProgram.LoadShaders("shaders/depth_map_cube/vertex.glsl", "shaders/depth_map_cube/fragment.glsl");

        // 5. Ustawienia macierzy rzutowania kamery oswietlenia
        matProj = glm::perspective(glm::radians(90.0f), DepthMap_Width / (float)DepthMap_Height, frustumNear, frustumFar);

        // aktualizacja macierzy view
        UpdateViewMat();
    }

    // Etap 2: Generowanie mapy cieni
    // pomiedzy tymi funkcjami renderujemy to co ma rzucac cien
    // i byc uwzglednione w mapie cieni
    // UWAGA! generowanie obiektow powinno odbywac sie za pomoca
    // programu idProgram z tego obiektu
    void GenBegin(int face)
    {
        glEnable(GL_DEPTH_TEST);
        glBindFramebuffer(GL_FRAMEBUFFER, idFrameBuffer[face]);
        glViewport(0, 0, DepthMap_Width, DepthMap_Height);
        glClear(GL_DEPTH_BUFFER_BIT);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, idTextureCube, 0);
        __CHECK_FOR_ERRORS

        // AKTYWUJEMY program
        ShadowPointLightProgram.Use();

        // Wysylanie macierzy
        ShadowPointLightProgram.SetMat4("matProj", matProj);
        ShadowPointLightProgram.SetMat4("matView", matViews[face]);
        ShadowPointLightProgram.SetFloat3("lightPos", lightPosition);
        ShadowPointLightProgram.SetFloat("farPlane", frustumFar);
    }

    void GenEnd(void)
    {

        // WYLACZAMY program, przechodzimy do domyslnego FBO
        ShadowPointLightProgram.UnUse();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    // Etap 3: Podczas finalnego renderingu na ekran
    // Wysylanie do programu _prog wszystkich potrzebnych uniformow
    // Ustawianie tekstury mapy cieni na slot _texSlot
    void SendUniforms(CProgram &_prog, GLuint _texSlot, glm::vec3 _camPos)
    {

        // Shadow map textura na slot _texSlot
        __CHECK_FOR_ERRORS
        glActiveTexture(GL_TEXTURE0 + _texSlot);
        glBindTexture(GL_TEXTURE_CUBE_MAP, idTextureCube);
        __CHECK_FOR_ERRORS
        _prog.SetInt("tex_shadowCubeMap", _texSlot);
        __CHECK_FOR_ERRORS

        _prog.SetFloat("farPlane", frustumFar);

        _prog.SetFloat3("cameraPos", _camPos);
        __CHECK_FOR_ERRORS
    }

    // Czyszczenie
    void Clean()
    {
        glDeleteProgram(idProgram);
        glDeleteTextures(1, &idTextureCube);
        glDeleteFramebuffers(6, &idFrameBuffer[0]);
    }
};

#endif