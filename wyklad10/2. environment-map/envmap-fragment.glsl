#version 330 core

// Standardowy fragment shader
// Tutaj ma znalezc sie wszystko to co potrzebne
// jest do wygenerowania obrazu, ktory bedzie 
// przechowany w Environment Mapie

// Dane odebrane z shadera wierzcholkow

in VS_OUT
{
	vec2 UV;
	vec3 Pos;
	vec3 Normal;

} frag;


out vec4 outColor;

void main()
{

	// Ewentualne tekstury 
	// ....
	// Ewentualne oswietlenie 
	// ...

	vec3 fragColor = vec3(0.0, 1.0, 0.0);


	// Kolor ostateczny
	outColor = vec4(fragColor, 1.0);
	
}
