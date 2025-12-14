#version 330 core

// Parametry oswietlenia oraz
// bryly obcinania swiatla
uniform vec3  lightPos;
uniform float farPlane;

in vec3 fragPos;
out vec4 outColor;

void main()
{
	// odleglosc fragmentu od zrodla swiatla
	float lightDistance = length(fragPos - lightPos);

	// znormalizowanie odleglosci do przedzialu [0,1]
	lightDistance = lightDistance / farPlane;

	// ten potok generuje tylko glebokosc
	// ktora odpowiednio preparyjemy
	// aby potem moc odczytac na potrzeby
	// wyliczania wspolczynnika cienia
	gl_FragDepth = lightDistance;

}
