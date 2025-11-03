// Fragment shader
#version 330 core

// Dane wyjsciowe
out vec4 outColor;

void main()
{
	float R = gl_PrimitiveID/7.0;
	float G = 1.0 - gl_PrimitiveID/7.0;
	float bright = 0.5+gl_FragCoord.y/600;
	
	outColor = vec4(R, G, 0.5, 1.0)*bright;
}

