#version 330 core
layout(location = 0) in vec4 aPos;
layout(location = 1) in vec2 aUV;
layout(location = 2) in vec3 aNormal;

out vec3 fragColor;

uniform mat4 matPV;
uniform mat4 matModel;

void main()
{
    fragColor = aNormal;
    gl_Position = matPV * matModel * aPos;
}
