#version 150 core

// zmienne wejsciowe
in vec4 inoutPos;
in vec3 inoutColor;
in vec2 inoutUV;

// ---------------------------------
// NOWE: uchwyt tekstury jako zmienna
// jednorodna typu sampler2D
// ---------------------------------
uniform sampler2D tex;
uniform sampler2D tex2;
uniform sampler2D tex3;

// zmienna wyjsciowa
out vec4 outColor;


void main()
{
	// ---------------------------------
	// NOWE: pobranie wartosci tekstury
	// dla zadanej wspolrzednej UV
	// ---------------------------------
	vec4 texColor1 = texture(tex, inoutUV);
	vec4 texColor2 = texture(tex2, inoutUV);
	vec4 texColor3 = texture(tex3, inoutUV);


	vec4 mix12 = mix(texColor1, texColor2, 0.5);
//	outColor = texColor2;
    outColor = mix(mix12, texColor3, 0.33);
}
