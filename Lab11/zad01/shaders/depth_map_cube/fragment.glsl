#version 330 core

in vec3 fragPos;

// Parametry oswietlenia oraz
// bryly obcinania swiatla
uniform vec3  lightPos;
uniform float farPlane;

void main()
{
	float lightDistance = length(fragPos - lightPos);

	// znormalizowanie odleglosci do przedzialu [0,1]
	lightDistance = lightDistance / farPlane;

	gl_FragDepth = lightDistance;

}
