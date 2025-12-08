#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;

uniform mat4 matModel;
uniform mat4 matView;
uniform mat4 matProj;
uniform vec3 cameraPos;

out vec2 TexCoord;
out vec3 inPosition;
out vec3 inNormal;

void main()
{
    // normals naprawiam
    inPosition = vec3(matModel * vec4(aPos, 1.0));
    inNormal = mat3(transpose(inverse(matModel))) * aNormal;

    TexCoord = aTexCoord;
    gl_Position = matProj * matView * matModel * vec4(aPos, 1.0);
}
