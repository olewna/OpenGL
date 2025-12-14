#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aUV;
layout(location = 2) in vec3 aNormal;

uniform mat4 matProj;
uniform mat4 matView;
uniform mat4 matModel;

out vec2 TexCoord;

void main()
{
    // TexCoord = aUV;
    gl_Position = matProj * matView * matModel * vec4(aPos, 1.0);
}
