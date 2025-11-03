#version 330 core
layout(location = 0) in vec2 inPosition;
uniform mat4 rotationMatrix;
        void main() {
            vec2 position = vec2(inPosition.y, -inPosition.x);
            vec4 pos = rotationMatrix * vec4(position, 0.0, 1.0);
            gl_Position = pos;
        }
