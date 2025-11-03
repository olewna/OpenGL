#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D tex0; // to co ustawiasz w Draw()

void main()
{
    vec4 color = texture(tex0, TexCoord);

    if (color.a < 0.1)  // próg przezroczystości
        discard;

    FragColor = color;
}
