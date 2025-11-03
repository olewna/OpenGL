// -------------------------------------------------
// Programowanie grafiki 3D w OpenGL / UG
// -------------------------------------------------
// Przyklad programu, ktory renderuje dane
// przy uzyciu tablicy indeksow.
// Nacisnij SPACE aby zmienic tryb.
// -------------------------------------------------
#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

// Funkcje pomocnicze
#include "utilities.hpp"


// ---------------------------------------
// Identyfikatory obiektow

GLuint idProgram;
GLuint idVBO_coord;
GLuint idVBO_color;
GLuint idVBO_indices;
GLuint idVAO;

// ---------------------------------------
// Wspolrzedne wierzchokow
GLfloat triangles_coord[2*6] =
{
	-0.5f, -1.0f,
	 0.5f, -1.0f,
	 1.0f,  0.0f,
	 0.5f,  1.0f,
	-0.5f,  1.0f,
	-1.0f,  0.0f
};

// ---------------------------------------
// Kolory wierzcholkow
GLfloat triangles_color[3*6] =
{
	1.f, 0.f, 0.f,
	0.f, 1.f, 0.f,
	0.f, 0.f, 1.f,
	1.f, 1.f, 0.f,
	0.f, 1.f, 1.f,
	1.f, 0.f, 1.f
};

// ---------------------------------------
// Indeksy wierzcholkow
GLuint triangles_indices[3*4] =
{
	0, 1, 2,
	0, 2, 3,
	0, 3, 4,
	0, 4, 5
};

// ---------------------------------------
// NOWE :
// Zmiena do kontroli trybu renderingu:
// za pomoca tablicy indeksow badz nie (po staremu)
bool renderingInstancyjny = false;

// ---------------------------------------
void DisplayScene()
{
	glClear( GL_COLOR_BUFFER_BIT );

	glUseProgram( idProgram );
	glBindVertexArray( idVAO );


		// Po staremu: Bez tablicy wierzcholkow
		if (renderingInstancyjny == false)
			glDrawArrays( GL_TRIANGLES, 0, 6 );
		else // Z tablica wierzcholkow
			glDrawElements( GL_TRIANGLES, 4 * 3, GL_UNSIGNED_INT, NULL );


	glBindVertexArray( 0 );
	glUseProgram( 0 );

	glutSwapBuffers();
}


// ---------------------------------------
void Reshape( int width, int height )
{
	glViewport( 0, 0, width, height );
}


// ---------------------------------------
void Initialize()
{

	glClearColor( 0.6f, 0.6f, 0.6f, 1.0f );


	// 1. Program i shadery
	idProgram = glCreateProgram();
	glAttachShader( idProgram, LoadShader(GL_VERTEX_SHADER, "vertex.glsl"));
	glAttachShader( idProgram, LoadShader(GL_FRAGMENT_SHADER, "fragment.glsl"));
	LinkAndValidateProgram( idProgram );


	// 2. Vertex arrays
	glGenVertexArrays( 1, &idVAO );
	glBindVertexArray( idVAO );

	// a) BUFOR: Wspolrzedne wierzcholkow
	glGenBuffers( 1, &idVBO_coord );
	glBindBuffer( GL_ARRAY_BUFFER, idVBO_coord );
	glBufferData( GL_ARRAY_BUFFER, sizeof( triangles_coord ), triangles_coord, GL_STATIC_DRAW );

	glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 0 );

	// b) BUFOR: Kolor wierzcholkow
	glGenBuffers( 1, &idVBO_color );
	glBindBuffer( GL_ARRAY_BUFFER, idVBO_color  );
	glBufferData( GL_ARRAY_BUFFER, sizeof( triangles_color ), triangles_color, GL_STATIC_DRAW );

	glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 1 );

	// ---------------------------------------
	// NOWE: Nowy bufor w ramach VAO potrzebny
	// do przechowywania tablicy indeksow
	// ---------------------------------------
	// c) BUFOR: Indeksy wierzcholkow
    glGenBuffers( 1, &idVBO_indices );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, idVBO_indices );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( triangles_indices ), triangles_indices, GL_STATIC_DRAW );


	glBindVertexArray( 0 );

}

// ---------------------------------------------------
// Funkcja wywolywana podczas wcisniecia klawisza ASCII
void Keyboard( unsigned char key, int x, int y )
{
    switch(key)
    {
		case 27:	// ESC key
			// opuszczamy glowna petle
			glutLeaveMainLoop();
			break;

		case ' ':	// Space key
			printf("Zmiana trybu renderingu\n");

			renderingInstancyjny = !renderingInstancyjny;

			// ponowne renderowanie
			glutPostRedisplay();
			break;


    }
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
	glutCreateWindow( "Tablice wierzcholkow - nacisnij SPACE" );
	glutDisplayFunc( DisplayScene );
	glutReshapeFunc( Reshape );
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
	glDeleteBuffers( 1, &idVBO_coord);
	glDeleteBuffers( 1, &idVBO_color );
	glDeleteBuffers( 1, &idVBO_indices );
	glDeleteVertexArrays( 1, &idVAO );

	return 0;
}
