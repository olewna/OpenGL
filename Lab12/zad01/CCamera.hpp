#ifndef __CCAMERA_HPP
#define __CCAMERA_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

glm::mat4 UpdateThirdPersonCamera(const CPlayer &player)
{
    glm::vec3 playerPos = player.GetPosition();
    float angle = player.GetAngle();

    float distance = 8.0f;
    float height = 5.0f;

    // kierunek w który patrzy gracz
    glm::vec3 forward;
    forward.x = cos(angle);
    forward.z = -sin(angle);
    forward.y = 0.0f;
    forward = glm::normalize(forward);

    // kamera za graczem
    glm::vec3 camPos = playerPos - forward * distance;
    camPos.y += height;

    matView = glm::lookAt(
        camPos,
        playerPos + glm::vec3(0, 1.5f, 0),
        glm::vec3(0, 1, 0));

    return matView;
}

#endif