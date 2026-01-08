#ifndef __MATERIALSPARAM_HPP
#define __MATERIALSPARAM_HPP

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Struktura materialu obiektu
struct MaterialParam
{
    glm::vec3 Ambient;
    glm::vec3 Diffuse;
    glm::vec3 Specular;
    float Shininess;
};

// ----------------------------------------------
// (2) MATERIAL
// ----------------------------------------------

// Przykladowy material
MaterialParam myMaterialMatowy = {
    glm::vec3(0.2, 0.2, 0.2), // ambient
    glm::vec3(1.0, 1.0, 1.0), // diffuse
    glm::vec3(1.0, 1.0, 1.0), // specular
    1.0                       // shininess
};
MaterialParam myMaterialBlysk = {
    glm::vec3(0.2, 0.2, 0.2), // ambient
    glm::vec3(1.0, 1.0, 1.0), // diffuse
    glm::vec3(1.0, 1.0, 1.0), // specular
    256.0                     // shininess
};

#endif