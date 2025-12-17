#ifndef __ENVIRONMENTAL_MAPPING
#define __ENVIRONMENTAL_MAPPING

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

GLuint idTextureColor;
GLuint idTextureDepth;
GLuint idFrameBuffer[6];

const float ENV_SIZE = 800.0f;
float envNear = 0.1f;
float envFar = 80.0f;

void InitEnvironmentMap()
{
    __CHECK_FOR_ERRORS
    // COLOR
    glGenTextures(1, &idTextureColor);
    glBindTexture(GL_TEXTURE_CUBE_MAP, idTextureColor);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    for (int i = 0; i < 6; i++)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, ENV_SIZE, ENV_SIZE, 0, GL_RGB, GL_FLOAT, nullptr);

    // DEPTH
    glGenTextures(1, &idTextureDepth);
    glBindTexture(GL_TEXTURE_CUBE_MAP, idTextureDepth);
    for (int i = 0; i < 6; i++)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT24, ENV_SIZE, ENV_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    // FBOs
    for (int i = 0; i < 6; i++)
    {
        glGenFramebuffers(1, &idFrameBuffer[i]);
        glBindFramebuffer(GL_FRAMEBUFFER, idFrameBuffer[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, idTextureColor, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, idTextureDepth, 0);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    __CHECK_FOR_ERRORS
}

void RenderEnvironmentMap(glm::vec3 objPos)
{
    __CHECK_FOR_ERRORS
    glm::mat4 proj = glm::perspective(glm::radians(100.0f), ENV_SIZE / ENV_SIZE, envNear, envFar);

    glm::mat4 views[6] = {
        glm::lookAt(objPos, objPos + glm::vec3(1, 0, 0), glm::vec3(0, -1, 0)),
        glm::lookAt(objPos, objPos + glm::vec3(-1, 0, 0), glm::vec3(0, -1, 0)),
        glm::lookAt(objPos, objPos + glm::vec3(0, 1, 0), glm::vec3(0, 0, 1)),
        glm::lookAt(objPos, objPos + glm::vec3(0, -1, 0), glm::vec3(0, 0, -1)),
        glm::lookAt(objPos, objPos + glm::vec3(0, 0, 1), glm::vec3(0, -1, 0)),
        glm::lookAt(objPos, objPos + glm::vec3(0, 0, -1), glm::vec3(0, -1, 0))};

    // glDisable(GL_CULL_FACE);

    for (int i = 0; i < 6; i++)
    {
        __CHECK_FOR_ERRORS
        glViewport(0, 0, ENV_SIZE, ENV_SIZE);
        glBindFramebuffer(GL_FRAMEBUFFER, idFrameBuffer[i]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        __CHECK_FOR_ERRORS

        glownyProgram.Use();
        __CHECK_FOR_ERRORS

        glownyProgram.SetMat4("matProj", proj);
        glownyProgram.SetMat4("matView", views[i]);
        __CHECK_FOR_ERRORS

        glownyProgram.SetInt("uUseEnvMap", 0);
        glownyProgram.SetFloat("envStrength", 0.0f);
        glownyProgram.SetMat4("matModel", plane.GetModelMatrix());
        glownyProgram.sendMaterialParameters(myMaterialMatowy);
        plane.Draw(glownyProgram);
        __CHECK_FOR_ERRORS

        glownyProgram.SetMat4("matModel", tower.GetModelMatrix());
        tower.Draw(glownyProgram);
        __CHECK_FOR_ERRORS

        glownyProgram.UnUse();
        __CHECK_FOR_ERRORS

        glDepthFunc(GL_LEQUAL);
        // glownyProgram.SetMat4("matView", matView);
        DrawSkyBox(views[i], proj);
        glDepthFunc(GL_LESS);
        __CHECK_FOR_ERRORS
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // glEnable(GL_CULL_FACE);

    int width, height;
    glfwGetFramebufferSize(glfwGetCurrentContext(), &width, &height);
    glViewport(0, 0, width, height);
    matProj = glm::perspective(glm::radians(80.0f), (float)width / (float)height, CameraNear, CameraFar);
}

void BindSkyboxTexture()
{
    __CHECK_FOR_ERRORS
    // ważne: nie koliduj z innymi texturami temu 10
    glActiveTexture(GL_TEXTURE10);
    glBindTexture(GL_TEXTURE_CUBE_MAP, idTextureColor);
    glownyProgram.SetInt("tex_environment", 10);
    __CHECK_FOR_ERRORS
}

#endif