#ifndef __CTEXTMESSAGE_HPP
#define __CTEXTMESSAGE_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

void showAllTexts(float currentFrame)
{
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);

    std::string fpsText = "FPS: " + std::to_string((int)fps);

    RenderText(
        fpsText,
        10.0f, // X ->
        10.0f, // Y  V
        2.0f,
        glm::vec3(1.0f, 0.0f, 0.0f)); // rgb kolor napisu

    std::string scoreText = "SCORE: " + std::to_string((int)score) + "/" + std::to_string((int)CEL_GRY);

    RenderText(
        scoreText,
        10.0f,     // X ->
        h - 50.0f, // Y  V
        2.0f,
        glm::vec3(0.0f, 0.0f, 1.0f));

    if (Time <= 2.5f)
    {
        std::string missionText = "COLLECT " + std::to_string((int)CEL_GRY) + " DONUTS!";

        float scale = 5.0f;

        float textWidth = missionText.length() * 25.0f * scale;
        float x = (w * 0.5f) - (textWidth * 0.25f);
        float y = h * 0.5f;
        float alpha = 0.5f + 0.5f * sin(currentFrame * 3.0f);

        RenderText(
            missionText,
            x,
            y,
            scale,
            glm::vec3(1.0f, alpha, 0.0f));
    }

    if (gameWon)
    {
        std::string winText = "YOU WIN!";

        float scale = 5.0f;

        float textWidth = winText.length() * 25.0f * scale;
        float x = (w * 0.5f) - (textWidth * 0.25f);
        float y = h * 0.5f;
        float alpha = 0.5f + 0.5f * sin(currentFrame * 3.0f);

        RenderText(
            winText,
            x,
            y,
            scale,
            glm::vec3(1.0f, alpha, 0.0f));
    }
}

#endif