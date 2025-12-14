#version 330 core
layout( location = 0 ) in vec4 inPosition;
layout( location = 1 ) in vec3 inNormal;

uniform mat4 matProj;
uniform mat4 matView;
uniform mat4 matModel;

out vec3 fragPos;
out vec3 fragNormal;



void main()
{
	gl_Position = matProj * matView * matModel * inPosition;

	// Polozenie wierzcholka w ukladzie sceny
	fragPos = (matModel * inPosition).xyz;

	// Macierz normalna powinna byc wyliczona na CPU
	// Tutaj tylko dla uproszczenia przykladu
	mat3 matNormal = mat3(transpose(inverse(matModel)));
	fragNormal =  matNormal * inNormal;
}
