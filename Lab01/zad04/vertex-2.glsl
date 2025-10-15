#version 330 core
layout(location = 0) in vec2 inPosition;
out float shade;

void main() {
    gl_Position = vec4(inPosition, 0.0, 1.0);
    shade = abs(inPosition.y); // odcienie zieleni w zależności od wysokości
}
