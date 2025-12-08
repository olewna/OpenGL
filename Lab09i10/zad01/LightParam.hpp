#ifndef __LIGHTPARAM_HPP
#define __LIGHTPARAM_HPP

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Struktura oswietlenia punktowego
struct LightParam
{
    glm::vec3 Ambient;
    glm::vec3 Diffuse;
    glm::vec3 Specular;
    glm::vec3 Attenuation;
    glm::vec3 Position; // Direction dla kierunkowego
    glm::vec3 Direction;
};

// ----------------------------------------------
// (1) SWIATLO
// ----------------------------------------------

// Przykladowe swiatlo punktowe
LightParam myLight1 = {
    glm::vec3(0.1, 0.1, 0.1),    // ambient
    glm::vec3(1.0, 1.0, 1.0),    // diffuse
    glm::vec3(1.0, 1.0, 1.0),    // specular
    glm::vec3(1.0, 0.0, 0.0),    // attenuation
    glm::vec3(0.0, 6.0, 0.0),    // position
    glm::vec3(0.0f, -1.0f, 0.0f) // direction
};
LightParam myLight2 = {
    glm::vec3(0.1, 0.1, 0.1),    // ambient
    glm::vec3(1.0, 0.0, 0.0),    // diffuse
    glm::vec3(1.0, 1.0, 1.0),    // specular
    glm::vec3(1.0, 0.0, 0.0),    // attenuation
    glm::vec3(2.0, 6.0, 2.0),    // position
    glm::vec3(0.0f, -1.0f, 0.0f) // direction
};
LightParam myLight3 = {
    glm::vec3(0.1, 0.1, 0.1),    // ambient
    glm::vec3(0.0, 1.0, 0.0),    // diffuse
    glm::vec3(1.0, 1.0, 1.0),    // specular
    glm::vec3(1.0, 0.0, 0.0),    // attenuation
    glm::vec3(2.0, 6.0, -2.0),   // position
    glm::vec3(0.0f, -1.0f, 0.0f) // direction
};
LightParam myLight4 = {
    glm::vec3(0.1, 0.1, 0.1),    // ambient
    glm::vec3(0.0, 0.0, 1.0),    // diffuse
    glm::vec3(1.0, 1.0, 1.0),    // specular
    glm::vec3(1.0, 0.0, 0.0),    // attenuation
    glm::vec3(-2.0, 6.0, 2.0),   // position
    glm::vec3(0.0f, -1.0f, 0.0f) // direction
};
LightParam myLight5 = {
    glm::vec3(0.1, 0.1, 0.1),    // ambient
    glm::vec3(1.0, 0.0, 1.0),    // diffuse
    glm::vec3(1.0, 1.0, 1.0),    // specular
    glm::vec3(1.0, 0.0, 0.0),    // attenuation
    glm::vec3(-2.0, 6.0, -2.0),  // position
    glm::vec3(0.0f, -1.0f, 0.0f) // direction
};

const int MAX_LIGHTS = 5;
int activeLights = 1;

LightParam lights[MAX_LIGHTS] = {myLight1, myLight2, myLight3, myLight4, myLight5};

#endif