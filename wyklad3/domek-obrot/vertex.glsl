#version 330 core

// Dane pobrane z VAO
layout (location = 0) in vec4 inPosition;

// Zmienne jednorodne do kontroli
// obrotem i translacja
uniform float Alpha = 0.0;
uniform float Move = 0.0;


void main()
{


	// Macierz obrotu o kat Alpha wokol osi Z
	mat4x4 MatrixR;
	MatrixR[0] = vec4( cos(Alpha), sin(Alpha), 0, 0);
	MatrixR[1] = vec4( -sin(Alpha), cos(Alpha), 0, 0);
	MatrixR[2] = vec4( 0, 0, 1, 0);
	MatrixR[3] = vec4( 0, 0, 0, 1);

	// Macierz translacji o wektor (Move, 0, 0)
	mat4x4 MatrixT;
	MatrixT[0] = vec4( 1, 0, 0, 0);
	MatrixT[1] = vec4( 0, 1, 0, 0);
	MatrixT[2] = vec4( 0, 0, 1, 0);
	MatrixT[3] = vec4( Move, 0, 0, 1);

	// Nowe polozenie wierzcholka (po uwzglednieniu
	// obrotu i translacji)
	vec4 newPosition = MatrixT * MatrixR * inPosition;

	// finalna pozycja wierzcholka
	gl_Position = newPosition;
	//gl_Position = inPosition;
}
