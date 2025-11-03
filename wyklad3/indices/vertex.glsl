#version 330

layout( location = 0 ) in vec4 inPosition;
layout( location = 1 ) in vec4 inColor;


// Do kolejnego etapu renderingu wysylamy zmienna inoutColor
out vec4 vertColor;


void main()
{
	// finalna pozycja wierzcholka
	gl_Position = inPosition;

	vertColor = inColor;
}
