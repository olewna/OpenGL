#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D tex0;
uniform float shift;

void main()
{
    vec2 uv = TexCoord;
    uv.x += shift;

    FragColor = texture(tex0, uv);
}
