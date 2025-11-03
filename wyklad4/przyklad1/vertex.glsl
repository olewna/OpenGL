// Vertex shader
#version 330 core

// Dane pobrane z VAO
layout (location = 0) in vec4 inPosition;
layout (location = 1) in vec3 inColor;

out vec3 fragColor;

uniform mat4 matProj;
uniform mat4 matView;
uniform mat4 matModel;


void main()
{
	vec4 newPosition = matProj * matView * matModel * inPosition;

	gl_Position = newPosition;
	fragColor = inColor;
}
