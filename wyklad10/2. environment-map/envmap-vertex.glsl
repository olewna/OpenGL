#version 330 core
layout( location = 0 ) in vec4 inPosition;
layout( location = 1 ) in vec3 inNormal;
layout( location = 2 ) in vec2 inUV;

// Dane wyslane do kolejnego etapu
in VS_OUT
{
	vec2 UV;
	vec3 Pos;
	vec3 Normal;

} vertex;

uniform mat4 matProj;
uniform mat4 matView;
uniform mat4 matModel;


void main()
{

	gl_Position = matProj * matView * matModel * inPosition;


	vertex.UV = inUV;
	vertex.Pos = (matModel * inPosition).xyz;
	vertex.Normal = inNormal;

}
