#version 330 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;

uniform mat4 model;

out vec3 vColor;

void main()
{
    gl_Position = model * vec4(inPosition, 1.0);

    vColor = vec3(
        (inPosition.x + 1.0) / 2.0,
        (inPosition.y + 1.0) / 2.0,
        (inPosition.z + 1.0) / 2.0
    );
}
