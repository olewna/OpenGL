#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D tex0;
uniform sampler2D frameTex;

void main()
{
    vec4 mapColor = texture(tex0, TexCoord);
    vec4 frameColor = texture(frameTex, TexCoord);

    FragColor = mix(mapColor, frameColor, frameColor.a);
    // FragColor = texture(tex0, TexCoord);
    // FragColor = vec4(TexCoord, 0.0, 1.0); 

}
