#version 330 core
layout (location = 0) in vec4 inPosition;

uniform mat4 matProj;
uniform mat4 matView;
uniform mat4 matModel;

out vec3 fragPos;

void main()
{
	// pozycja wierzcholka w ukladzie sceny
	// potrzebna do wyliczenia odleglosci
	// od zrodla swiatla we fragment shaderze
	fragPos = (matModel * inPosition).xyz;
	
	gl_Position = matProj * matView * matModel * inPosition;
}
