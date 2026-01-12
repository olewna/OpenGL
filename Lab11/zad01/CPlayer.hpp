#ifndef __CPLAYER_HPP
#define __CPLAYER_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "CProgram.hpp"
#include "CMesh.hpp"
#include "CGround.hpp"

class CPlayer
{
public:
    // Domyslny konstruktor
    CPlayer() = default;

    void Init(const CGround *ground, const char *objFile, const char *texFile)
    {
        myGround = ground;
        if (!mesh.CreateFromOBJ(objFile))
        {
            std::cerr << "Player mesh not loaded!\n";
        }

        mesh.LoadTexture(texFile);

        Update();
    }

    void Update()
    {
        if (myGround)
        {
            Position.y = myGround->getYForPlayer(glm::vec2(Position.x, Position.z), Position.y);
        }

        // aktualizacja macierzy modelu
        glm::mat4 model(1.0f);
        model = glm::translate(model, Position);
        model = glm::rotate(model, Angle, glm::vec3(0, 1, 0));

        mesh.SetPosition(Position);
        mesh.SetRotation(glm::vec3(0.0f, glm::degrees(Angle), 0.0f));
    }

    void Draw(CProgram &program)
    {
        program.SetMat4("matModel", mesh.GetModelMatrix());
        mesh.Draw(program);
    }

    void SetPosition(const glm::vec3 &pos)
    {
        Position = pos;
        Update();
    }

    void Move(float speed)
    {
        // Position += Direction * speed;
        // Update();

        glm::vec3 nextPos = Position + Direction * speed;

        if (myGround)
        {
            float groundY = myGround->getYForPlayer(glm::vec2(nextPos.x, nextPos.z), Position.y);

            if (std::isnan(groundY))
                return;

            nextPos.y = groundY;
        }

        Position = nextPos;
        Update();
    }

    void Rotate(float angle)
    {
        Angle += angle;

        Direction.x = cos(Angle);
        Direction.z = -sin(Angle);

        Update();
    }

private:
    glm::vec3 Position = glm::vec3(0.0f);
    float Angle = 0.0f;
    glm::vec3 Direction = glm::vec3(1.0f, 0.0f, 0.0f);
    const CGround *myGround = nullptr;
    CMesh mesh;
};

#endif