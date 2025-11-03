#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D tex0; // to co ustawiasz w Draw()

void main()
{
    vec4 color = texture(tex0, TexCoord);

    // Jeœli piksel jest czerwony, odrzuæ go
    float threshold = 0.0005; // tolerancja
    if(color.r > 0.2 && color.g < threshold && color.b < threshold)
        discard;

    FragColor = color;
}
