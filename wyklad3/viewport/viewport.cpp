// -------------------------------------------------
// Programowanie grafiki 3D w OpenGL / UG
// -------------------------------------------------
// Prosta demonstracja uzycia funkcji glViewport,
// ktora ustala obszar renderingu. W ten sposob
// mozemy utworzyc kilka obszarow renderingu, np.
// dwa ktore wypelniaja okno poziomo po polowie.
// -------------------------------------------------
#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

// Funkcje pomocnicze
#include "utilities.hpp"


// ---------------------------------------
// Identyfikatory obiektow OpenGL

GLuint idProgram;	// poroku/programu
GLuint idVAO;		// tablicy wierzcholkow
GLuint idVBO_coord; // bufora na wspolrzedne wierzcholkow

int Window_Width = 500;
int Window_Height = 500;

// ---------------------------------------
// Dane geometrii

// Wspolrzedne wierzchokow
// po dwa floaty na wierzcholek
// wspolrzedne w zakresie -1.0 do 1.0
GLfloat triangles[6*2] =
{
	-0.8, -0.8,	// wierzcholek 0
	 0.0, -0.8, // wierzcholek 1
	-0.4,  0.0, // itd.

	 0.0,  0.0,
	 0.8,  0.0,
	 0.4,  0.8
};


// ---------------------------------------
void DisplayScene()
{
	glClear( GL_COLOR_BUFFER_BIT );

	// -----------------------------
	// NOWE: Pierwszy viewport
	// -----------------------------
	glViewport( 0, 0, Window_Width/2, Window_Height );

		glUseProgram( idProgram );
			glBindVertexArray( idVAO );
			glDrawArrays( GL_TRIANGLES, 0, 6 );
			glBindVertexArray( 0 );
		glUseProgram( 0 );

	// -----------------------------
	// NOWE: Drugi viewport
	// -----------------------------
	glViewport( Window_Width/2, 0, Window_Width/2, Window_Height );

		glUseProgram( idProgram );
			glBindVertexArray( idVAO );
			glDrawArrays( GL_TRIANGLES, 0, 6 );
			glBindVertexArray( 0 );
		glUseProgram( 0 );

	glutSwapBuffers();
}

// ---------------------------------------
void Initialize()
{
	// -------------------------------------------------
	// Etap (2) przeslanie danych wierzcholków do OpenGL
	// -------------------------------------------------

	// Tworzymy tablice wierzcholkow (VAO)
	glGenVertexArrays( 1, &idVAO );
	glBindVertexArray( idVAO );
		// Tworzymy bufor na wspolrzedne wierzcholkow
		glGenBuffers( 1, &idVBO_coord );
		glBindBuffer( GL_ARRAY_BUFFER, idVBO_coord );
		glBufferData( GL_ARRAY_BUFFER, sizeof(GLfloat) * (6*2), triangles, GL_STATIC_DRAW );
		// Ustawienie danych bufora pod atrybut o numerze 0
		// ten sam ustawiony w vertex shaderze
		glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 0, NULL );
		glEnableVertexAttribArray( 0 );
	glBindVertexArray( 0 );


	// ---------------------------------------
	// Etap (3) stworzenie potoku graficznego
	// ---------------------------------------
	idProgram = glCreateProgram();
		glAttachShader( idProgram, LoadShader(GL_VERTEX_SHADER, "vertex.glsl"));
		glAttachShader( idProgram, LoadShader(GL_FRAGMENT_SHADER, "fragment.glsl"));
	LinkAndValidateProgram( idProgram );



	// -----------------------------------------
	// Etap (4) ustawienie maszyny stanow OpenGL
	// -----------------------------------------

	// ustawienie koloru czyszczenia ramki koloru
	glClearColor( 0.9f, 0.9f, 0.9f, 0.9f );

}

// ---------------------------------------------------
// Funkcja wywolywana podczas tworzenia okna aplikacji
// oraz zmiany jego rozdzielczosci
void Reshape( int width, int height )
{

	//Stare: glViewport( 0, 0, width, height );
	// Nowe:
	// Aktualizujemy rozmiar okna potrzebny do funkcji glViewport
	// ktora jest wywolywana w funkcji DisplayScene()
	Window_Width = width;
	Window_Height = height;
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
			printf("SPACE!\n");
			// ponowne renderowanie
			glutPostRedisplay();
			break;

		case 'x':
			// opuszczamy glowna petle
			glutLeaveMainLoop();
			break;
    }
}


// ---------------------------------------------------
int main( int argc, char *argv[] )
{
	// -----------------------------------------------
	// Etap (1) utworzynie kontektu OpenGLa i okna
	// aplikacji, rejestracja funkcji zwrotnych
	// -----------------------------------------------

	// GLUT
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB );
	glutInitContextVersion( 3, 3 );
	glutInitContextProfile( GLUT_CORE_PROFILE );
	glutInitWindowSize( Window_Width, Window_Height );
	glutCreateWindow( "Szablon programu w OpenGL" );
	glutDisplayFunc( DisplayScene );
	glutReshapeFunc( Reshape );
	glutKeyboardFunc( Keyboard );


	// GLEW
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if( GLEW_OK != err ) {
		printf("GLEW Error\n");
		exit(1);
	}

	// OpenGL
	if( !GLEW_VERSION_3_3 ) {
		printf("Brak OpenGL 3.3!\n");
		exit(1);
	}


	// Inicjalizacja
	Initialize();

	// Rendering
	glutMainLoop();


	// Cleaning
	glDeleteProgram( idProgram );
	glDeleteVertexArrays( 1, &idVBO_coord );
	glDeleteVertexArrays( 1, &idVAO );

	return 0;
}
