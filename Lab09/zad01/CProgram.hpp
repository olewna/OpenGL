#ifndef __CPROGRAM_HPP
#define __CPROGRAM_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "utilities.hpp"

// =======================================================
// 1. Klasa do obslugi potoku
// =======================================================
class CProgram
{
public:
    CProgram() : idProgram(0) {}
    ~CProgram() { Clean(); }

    bool Init()
    {
        idProgram = glCreateProgram();
        return idProgram != 0;
    }

    void Clean()
    {
        if (idProgram)
        {
            glDeleteProgram(idProgram);
            idProgram = 0;
        }
    }

    bool LoadShaders(const char *vertexFile, const char *fragmentFile)
    {
        glAttachShader(idProgram, LoadShader(GL_VERTEX_SHADER, vertexFile));
        glAttachShader(idProgram, LoadShader(GL_FRAGMENT_SHADER, fragmentFile));
        LinkAndValidateProgram(idProgram);
        return true;
    }

    void Use() { glUseProgram(idProgram); }
    void UnUse() { glUseProgram(0); }

    void SetMat4(const char *name, const glm::mat4 &matrix)
    {
        glUniformMatrix4fv(glGetUniformLocation(idProgram, name), 1, GL_FALSE, glm::value_ptr(matrix));
    }
    void SetInt(const char *name, int value)
    {
        glUniform1i(glGetUniformLocation(idProgram, name), value);
    }

    void SetFloat(const char *name, float value)
    {
        glUniform1f(glGetUniformLocation(idProgram, name), value);
    }

    void SetShadingProgram()
    {
        glUniform1i(glGetUniformLocation(idProgram, "shadingModel"), shadingModel);
    }

    // Przeslanie parametrow oswietlenia do shadera
    void sendLightParameters(const std::string &name, LightParam light)
    {
        // pobranie id aktualnego programu
        GLint programId = idProgram;
        glGetIntegerv(GL_CURRENT_PROGRAM, &programId);

        glUniform3fv(glGetUniformLocation(programId, (name + "Ambient").c_str()), 1, glm::value_ptr(light.Ambient));
        glUniform3fv(glGetUniformLocation(programId, (name + "Diffuse").c_str()), 1, glm::value_ptr(light.Diffuse));
        glUniform3fv(glGetUniformLocation(programId, (name + "Specular").c_str()), 1, glm::value_ptr(light.Specular));
        glUniform3fv(glGetUniformLocation(programId, (name + "Attenuation").c_str()), 1, glm::value_ptr(light.Attenuation));
        glUniform3fv(glGetUniformLocation(programId, (name + "Position").c_str()), 1, glm::value_ptr(light.Position));
        glUniform3fv(glGetUniformLocation(programId, (name + "Direction").c_str()), 1, glm::value_ptr(light.Direction));
    }

    // Przeslanie parametrow materialow do shadera
    void sendMaterialParameters(MaterialParam material)
    {
        // pobranie id aktualnego programu
        GLint programId = idProgram;
        glGetIntegerv(GL_CURRENT_PROGRAM, &programId);

        glUniform3fv(glGetUniformLocation(programId, "myMaterial.Ambient"), 1, glm::value_ptr(material.Ambient));
        glUniform3fv(glGetUniformLocation(programId, "myMaterial.Diffuse"), 1, glm::value_ptr(material.Diffuse));
        glUniform3fv(glGetUniformLocation(programId, "myMaterial.Specular"), 1, glm::value_ptr(material.Specular));
        glUniform1f(glGetUniformLocation(programId, "myMaterial.Shininess"), material.Shininess);
    }

    void SetLightingUniforms()
    {
        GLint loc = glGetUniformLocation(idProgram, "uLightingEnabled");
        if (loc != -1)
            glUniform1i(loc, lightingEnabled ? 1 : 0);
    }

    void SetCameraUniform()
    {
        glm::vec3 cameraPos = ExtractCameraPos(matView);

        glUniform3fv(glGetUniformLocation(idProgram, "cameraPos"), 1, glm::value_ptr(cameraPos));
    }

    void SetDrawLightSphere(bool v)
    {
        glUniform1i(glGetUniformLocation(idProgram, "uDrawLightSphere"), v ? 1 : 0);
    }

    GLuint GetId() const
    {
        return idProgram;
    }

private:
    GLuint idProgram;
};

#endif