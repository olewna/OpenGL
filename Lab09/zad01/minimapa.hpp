#ifndef __MINIMAPA_HPP
#define __MINIMAPA_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "CProgram.hpp"

GLuint fboMiniMap, texMiniMap, rboMiniMap;
GLuint vaoMiniMap, vboMiniMap;
int minimapSize = 500;
CProgram minimapProgram;

// =======================================================
// INIT FBO MINIMAPY
// =======================================================
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

    minimapProgram.Init();
    minimapProgram.LoadShaders("shaders/minimap/minimap_vertex.glsl", "shaders/minimap/minimap_fragment.glsl");

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

#endif