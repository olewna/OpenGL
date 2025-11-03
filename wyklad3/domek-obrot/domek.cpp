// -------------------------------------------------
// Programowanie grafiki 3D w OpenGL / UG
// -------------------------------------------------
// Przyklad programu, w ktorym mozemy obracac
// obiektem klawiszami W i S oraz przesuwaniem
// klawiszami A i D na osi X. Sam obrot
// odbywa sie w shaderze wierzcholkow.
// -------------------------------------------------
#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>


#include "utilities.hpp"
#include "house.h"


// Identyfikatory obiektow
GLuint idProgram;	// programu
GLuint idVAO;		// tablic wierzcholkow
GLuint idVBO_coord;	// bufora wierzcholkow
GLuint idVBO_color; // bufora kolorow


// ---------------------------------------
// NOWE: zmienne sluzace do kontroli
// obrotu i translacji obiektu
// ---------------------------------------
float Alpha = 0.0;
float Move = 0.0;

// ---------------------------------------
void DisplayScene()
{
    std::cout << Alpha << ", " << Move << std::endl;

	glClear( GL_COLOR_BUFFER_BIT );
	glUseProgram( idProgram );


		// --------------------------------------------
		// Ustawienie wartosci zmiennych jednorodnych
		// w potoku na potrzeby obrotu i translacji.
		// --------------------------------------------
		glUniform1f(glGetUniformLocation(idProgram, "Alpha"), Alpha);
		glUniform1f(glGetUniformLocation(idProgram, "Move"), Move);

		// Generowanie obiektow na ekranie
		glBindVertexArray( idVAO );
		glDrawArrays( GL_TRIANGLES, 0, 9*3 );
		glBindVertexArray( 0 );


	glUseProgram( 0 );


	glutSwapBuffers();
}

// ---------------------------------------
void Initialize()
{
	// Kolor czyszczenia bufora ramki koloru
	glClearColor( .2f, .2f, .2f, .2f );


	// Program i shadery
	idProgram = glCreateProgram();
	glAttachShader( idProgram, LoadShader(GL_VERTEX_SHADER, "vertex.glsl"));
	glAttachShader( idProgram, LoadShader(GL_FRAGMENT_SHADER, "fragment.glsl"));
	LinkAndValidateProgram( idProgram );

	// 2. Vertex arrays
	glGenVertexArrays( 1, &idVAO );
	glBindVertexArray( idVAO );
		// Bufor na wspolrzedne wierzcholkow
		glGenBuffers( 1, &idVBO_coord );
		glBindBuffer( GL_ARRAY_BUFFER, idVBO_coord );
		glBufferData( GL_ARRAY_BUFFER, sizeof(Mesh_Vertices), Mesh_Vertices, GL_STATIC_DRAW );
		glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 0, NULL );
		glEnableVertexAttribArray( 0 );
	glBindVertexArray( 0 );

}

// ---------------------------------------
void Reshape( int width, int height )
{
	glViewport( 0, 0, width, height );
}

// ---------------------------------------------------
void Keyboard( unsigned char key, int x, int y )
{
    switch(key)
    {
		case 27:	// ESC key
			glutLeaveMainLoop();
			break;

		case 'w':
			Alpha -= 0.1;
			break;

		case 's':
			Alpha += 0.1;
			break;

		case 'a':
			Move -= 0.1;
			break;

		case 'd':
			Move += 0.1;
			break;

    }

    glutPostRedisplay();
}


// ---------------------------------------------------
int main( int argc, char *argv[] )
{
	// GLUT
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB );
	glutInitContextVersion( 3, 3 );
	glutInitContextProfile( GLUT_CORE_PROFILE );
	glutInitWindowSize( 500, 500 );
	glutCreateWindow( "Obrot i translacja - klawisze WSAD" );
	glutDisplayFunc( DisplayScene );
	glutReshapeFunc( Reshape );

	// Keyboard
	glutKeyboardFunc( Keyboard );


	// GLEW
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if( GLEW_OK != err )
	{
		printf("GLEW Error\n");
		exit(1);
	}

	// OpenGL
	if( !GLEW_VERSION_3_3 )
	{
		printf("Brak OpenGL 3.3!\n");
		exit(1);
	}


	Initialize();

	glutMainLoop();


	// Cleaning
	glDeleteProgram( idProgram );
	glDeleteBuffers( 1, &idVBO_coord );
	glDeleteVertexArrays( 1, &idVAO );

	return 0;
}

