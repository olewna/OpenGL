// -------------------------------------------------------
// Programowanie grafiki 3D w OpenGL / UG
// -------------------------------------------------------
// Przyklad dla tematow
// - cienie metoda shadow mapping
// - cienie swiatla punktowego
// - renderowanie pozaekranowo do tekstury szesciennej
// -------------------------------------------------------
// References:
// - https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping
// - https://learnopengl.com/Advanced-Lighting/Shadows/Point-Shadows
// - Janusz Ganczarski, OpenGL. Podstawy programowania grafiki 3D, Helion.
// -------------------------------------------------------
#include <stdio.h>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Biblioteki GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Lokalne pliki naglowkowe
#include "utilities.hpp"
#include "objloader.hpp"

// -------------------------------------------------------
// NOWE: Pliki mechanizmu oswietlenia punktowego
// -------------------------------------------------------
#include "shadow-point.hpp"
CShadowPointLight myLight;


// Okno aplikacji
int windowWidth = 800, windowHeight = 800;
const char *windowTitle = "OpenGL (shadow mapping - nacisnij SPACE)";


// Identyfikatory
GLuint idProgram;
GLuint idVAO;
GLuint idVBO_pos;
GLuint idVBO_normals;

// Macierze
glm::mat4x4 matProj;
glm::mat4x4 matView;
glm::mat4x4 matModel = glm::mat4x4(1.0);


// Do pliku OBJ
std::vector<glm::vec3> OBJ_vertices;
std::vector<glm::vec3> OBJ_normals;
std::vector<glm::vec2> OBJ_uvs;

bool isShadowMapping = false;


// -------------------------------------------------------
void RenderScene_to_ShadowCubeMap()
{
	// -------------------------------------------------------
	// NOWE: Generowanie szesciennej mapy cieni
	// -------------------------------------------------------

	// Uruchomienie programu do generowania mapy cieni
	// Przeslanie wszystkich potrzebnych informacji do shaderow
	// Ukrywamy to wszystko w metodzie GenBegin()
	// To ona ma wiedziec co robic

	// Renderowanie 6 razy do kazdej sciany cube mapy
	// Gdy poznamy shader geometrii, ten etap zostanie zamieniony
	// na jedno przejscie
	for (int face=0; face < 6; face++)
	{
		myLight.GenBegin(face);


			// Renderujemy obiekt przy uzyciu aktualnego programu
			// Koniecznie wysylamy macierz modelu, aby potok wyrenderowal obiekt
			// w odpowiednim miejscu sceny
			glUniformMatrix4fv( glGetUniformLocation( myLight.idProgram, "matModel" ), 1, GL_FALSE, glm::value_ptr(matModel) );

			glBindVertexArray( idVAO );
			glDrawArrays( GL_TRIANGLES, 0, OBJ_vertices.size() );
			glBindVertexArray( 0 );

			// NOWE INNE OBIEKTY, KTORE MAJA RZUCAC CIEN
			// pamietajmy aby renderowac je za pomoca programu
			// o identyfikatorze myLight.idProgram
			// macierze modelu itd.
	}

	// Zakonczenie procesu generowania cieni
	myLight.GenEnd();
}


// -------------------------------------------------------
void RenderScene_on_Screen()
{
	// Przelaczamy sie na domyslny frame buffer
	glViewport(0, 0, windowWidth, windowHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	// Obliczanie macierzy widoku
	matView = UpdateViewMatrix();
	// Wyliczanie pozycji kamery z matView
	glm::vec3 cameraPos = ExtractCameraPos(matView);


	// AKTYWUJEMY program
	glUseProgram( idProgram );


		glUniformMatrix4fv( glGetUniformLocation( idProgram, "matProj" ), 1, GL_FALSE, glm::value_ptr(matProj) );
		glUniformMatrix4fv( glGetUniformLocation( idProgram, "matView" ), 1, GL_FALSE, glm::value_ptr(matView) );
		glUniformMatrix4fv( glGetUniformLocation( idProgram, "matModel" ), 1, GL_FALSE, glm::value_ptr(matModel) );


		// -------------------------------------------------------
		// NOWE: Wyslij wszystko co potrzebuje CShadowPointLight
		// -------------------------------------------------------
		myLight.SendUniforms(idProgram, 1, cameraPos);


		// Scena
		glBindVertexArray( idVAO );
		glDrawArrays( GL_TRIANGLES, 0, OBJ_vertices.size() );
		glBindVertexArray( 0 );


    // WYLACZAMY program
    glUseProgram(0);

}

// -------------------------------------------------------
void DisplayScene()
{
	// 1. Rendering pozaekranowy do mapy cienia
	if (isShadowMapping)
		RenderScene_to_ShadowCubeMap();


	// 2. Rendering na ekran z polozenia kamery
	RenderScene_on_Screen();

	// Pamietajmy o sprawdzeniu ew. bledow
	// na potrzeby budowania projektu
	__CHECK_FOR_ERRORS
}



// ---------------------------------------------------
void Initialize()
{
	// Obliczanie matProj po raz pierwszy
	if (windowHeight != 0)
		matProj = glm::perspective(glm::radians(70.0f), windowWidth/(float)windowHeight, 0.1f, 100.0f );

	// Ustawienia OpenGL i sceny
	glEnable( GL_DEPTH_TEST );
	glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );

	// Ladowanie pliku OBJ
	if (!loadOBJ("assets/scene.obj", OBJ_vertices, OBJ_uvs, OBJ_normals))
	{
		printf("Not loaded!\n");
		exit(1);
	}

	// Tworzenie potoku OpenGL
	idProgram = glCreateProgram();
	glAttachShader( idProgram, LoadShader(GL_VERTEX_SHADER, "shaders/vertex.glsl"));
	glAttachShader( idProgram, LoadShader(GL_FRAGMENT_SHADER, "shaders/fragment.glsl"));
	LinkAndValidateProgram( idProgram );

	// SCENA
	glGenVertexArrays( 1, &idVAO );
	glBindVertexArray( idVAO );
		glGenBuffers( 1, &idVBO_pos );
		glBindBuffer( GL_ARRAY_BUFFER, idVBO_pos );
		glBufferData( GL_ARRAY_BUFFER, OBJ_vertices.size() * sizeof(glm::vec3), &(OBJ_vertices)[0], GL_STATIC_DRAW );
		glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
		glEnableVertexAttribArray( 0 );
		glGenBuffers( 1, &idVBO_normals );
		glBindBuffer( GL_ARRAY_BUFFER, idVBO_normals );
		glBufferData( GL_ARRAY_BUFFER, OBJ_normals.size() * sizeof(glm::vec3), &(OBJ_normals)[0], GL_STATIC_DRAW );
		glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 1 );


	// -------------------------------------------------------
	// NOWE: Inicjalizacja oswietlenia punktowego w ustalonym polozeniu
	// -------------------------------------------------------
	myLight.Init(glm::vec3(0.0, 5.0, 0.0));

}





// ---------------------------------------------------
int main( int argc, char *argv[] )
{
	// Kontekst i okno aplikacji
	GLFWwindow* window = nullptr;
	Initialize_GLFW(window);

	// Inicjalizacja sceny
	Initialize();


	// Glowna petla
	while (!glfwWindowShouldClose(window))
	{
		// Obsluga zdarzen
		glfwPollEvents();

		// Sprawdzanie bledow
		__CHECK_FOR_ERRORS

		// Render Sceny
		DisplayScene();

		glfwSwapBuffers(window);
	}

	// Cleaning
	glDeleteProgram( idProgram );
	glDeleteVertexArrays( 1, &idVAO );
	glDeleteBuffers(1, &idVBO_pos);
	glDeleteBuffers(1, &idVBO_normals);

	// -----------------------------------------
	// NOWE: Czyszczenie tego co stworzylismy
	// w ramach shadow mappingu
	// -----------------------------------------
	myLight.Clean();

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);

	return 0;
}
