#ifndef __SHADOWMAPPING_HPP
#define __SHADOWMAPPING_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "CProgram.hpp"
#include "LightParam.hpp"

GLuint depthMapFBO, depthMapTex;
CProgram shadowMapProgram;
glm::mat4 lightProj;
glm::mat4 lightView;
const GLuint SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

void DirectionalLightCamera()
{
    glm::vec3 lightDir = normalize(myLight1.Direction);
    glm::vec3 lightPos = myLight1.Position;

    glm::vec3 target = lightPos + lightDir;
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    lightProj = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, 2.0f, 30.5f);
    lightView = glm::lookAt(lightPos, target, up);
}

void InitShadowMap()
{
    DirectionalLightCamera();

    // tworzenie tekstury obrazu
    glGenTextures(1, &depthMapTex);
    glBindTexture(GL_TEXTURE_2D, depthMapTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = {0.0f, 0.0f, 0.0f, 0.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    // tworzenie obiektu ramki bufora
    glGenFramebuffers(1, &depthMapFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

    // dolaczenie tekstury do ramki bufora
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTex, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    shadowMapProgram.Init();
    shadowMapProgram.LoadShaders("shaders/shadow_map/vertex.glsl", "shaders/shadow_map/fragment.glsl");
}

void RenderShadowMap()
{
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    shadowMapProgram.Use();
    // shadowMapProgram.SetMat4("lightSpaceMatrix", lightSpaceMatrix);
    shadowMapProgram.SetMat4("matProj", lightProj);
    shadowMapProgram.SetMat4("matView", lightView);

    shadowMapProgram.SetMat4("matModel", plane.GetModelMatrix());
    plane.Draw(shadowMapProgram);

    shadowMapProgram.SetMat4("matModel", tower.GetModelMatrix());
    tower.Draw(shadowMapProgram);

    shadowMapProgram.SetMat4("matModel", monkey.GetModelMatrix());
    monkey.Draw(shadowMapProgram);

    shadowMapProgram.UnUse();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderShadowMapOnScreen()
{
    glownyProgram.SetMat4("lightProj", lightProj);
    glownyProgram.SetMat4("lightView", lightView);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, depthMapTex);
    glownyProgram.SetInt("texture_shadowMap", 2);
}

#endif