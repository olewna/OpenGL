#version 330 core
layout(location = 0) in vec2 inPos;
uniform int Mode;
out vec3 vColor;

void main() {
    vec2 pos = inPos;

    if (Mode == 0) {
        pos *= 0.5;
        vColor = vec3(1.0, 0.0, 0.0);
    } else if (Mode == 1) {
        pos = vec2(-pos.x, pos.y);
        vColor = vec3(0.0, 1.0, 0.0);
    } else if (Mode == 2) {
        pos = vec2(pos.x, -pos.y);
        vColor = vec3(0.0, 0.0, 1.0);
    } else {
        pos *= 1.2;
        vColor = vec3(1.0, 1.0, 0.0);
    }

    gl_Position = vec4(pos, 0.0, 1.0);
}
