#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D tex0;
uniform int frameIndex;      // która klatka
uniform int totalFrames;     // ile klatek w texturze

void main()
{
    vec2 uv = TexCoord;

    // przesunięcie UV w poziomie w zależności od klatki
    float frameWidth = 1.0 / float(totalFrames);
    uv.x = uv.x * frameWidth + frameWidth * float(frameIndex);

    FragColor = texture(tex0, uv);

    if (FragColor.a < 0.1)
        discard;
}
