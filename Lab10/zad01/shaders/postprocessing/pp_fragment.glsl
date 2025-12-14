#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D screenTexture;
uniform int effect;

void main()
{
    vec3 color = texture(screenTexture, TexCoord).rgb;

    if (effect == 0)
    {
        // NEGATIVE
        color = vec3(1.0) - color;
    }
    else if (effect == 1)
    {
        // SHARPEN (kernal convolutions)
        float offset = 1.0 / 720.0;

        vec2 offsets[9] = vec2[](
            vec2(-offset,  offset),
            vec2( 0.0f,    offset),
            vec2( offset,  offset),
            vec2(-offset,  0.0f  ),
            vec2( 0.0f,    0.0f  ),
            vec2( offset,  0.0f  ),
            vec2(-offset, -offset),
            vec2( 0.0f,   -offset),
            vec2( offset, -offset)
        );

        float kernel[9] = float[](
            -1, -1, -1,
            -1,  9, -1,
            -1, -1, -1
        );

        vec3 sampleTex[9];
        for (int i = 0; i < 9; i++)
            sampleTex[i] = texture(screenTexture, TexCoord + offsets[i]).rgb;

        vec3 col = vec3(0.0);
        for (int i = 0; i < 9; i++)
            col += sampleTex[i] * kernel[i];

        color = col;
    }

    FragColor = vec4(color, 1.0);
}
