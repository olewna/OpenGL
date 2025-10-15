#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Pliki naglowkoweg OpenGLa
#include <GL/glew.h>
#include <GL/freeglut.h>


// Identyfikatory obiektow
GLuint idProgram;	// potoku/programu
GLuint idVBO;		// bufora wierzcholkow
GLuint idVAO;		// tablic wierzcholkow

const int N = 100;  // liczba trójkątów

// Naglowki naszych funkcji
void DisplayScene();
void Initialize();
void CreateVertexShader( void );
void CreateFragmentShader( void );
void Reshape( int width, int height );




// ---------------------------------------------------
int main( int argc, char *argv[] )
{
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB );
	glutInitContextVersion( 3, 3 );
	glutInitContextProfile( GLUT_CORE_PROFILE );
	glutInitWindowSize( 800, 800 );
	glutCreateWindow( "Zad02" );

	glutDisplayFunc( DisplayScene );
	glutReshapeFunc( Reshape );

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if( GLEW_OK != err )
	{
		printf("GLEW Error\n");
		return -1;
	}

	if( !GLEW_VERSION_3_3 )
	{
		printf("Brak OpenGL 3.3!\n");
		return -1;
	}

	Initialize();
	glutMainLoop();

	glDeleteProgram( idProgram );
	glDeleteBuffers( 1, &idVBO );
	glDeleteVertexArrays( 1, &idVAO );

	return 0;
}

void DisplayScene()
{
	glClear( GL_COLOR_BUFFER_BIT );
	glUseProgram( idProgram );
    glBindVertexArray( idVAO );

    glDrawArrays( GL_TRIANGLES, 0, 3 * N );

    glBindVertexArray( 0 );
	glUseProgram( 0 );
	glutSwapBuffers();
}

void Initialize()
{
    srand(time(NULL)); //SEED dla rand inta

	glGenVertexArrays( 1, &idVAO );
	glBindVertexArray( idVAO );

	glGenBuffers( 1, &idVBO );
	glBindBuffer( GL_ARRAY_BUFFER, idVBO );

	GLfloat *triangles = new GLfloat[3 * N * 2];

	for (int i = 0; i < N; i++) {
        float x = ((float)rand() / RAND_MAX) * 2.0f - 1.0f; //od -1 do 1
        float y = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
        float size = 0.05f + ((float)rand() / RAND_MAX) * 0.05f; // od 0.05 do 0.1

        triangles[i * 6 + 0] = x - size; triangles[i * 6 + 1] = y - size;
        triangles[i * 6 + 2] = x + size; triangles[i * 6 + 3] = y - size;
        triangles[i * 6 + 4] = x; triangles[i * 6 + 5] = y + size;
	}

	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * N * 2, triangles, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    delete[] triangles;

	glBindVertexArray( 0 );

	idProgram = glCreateProgram();
	CreateVertexShader();
	CreateFragmentShader();

	glLinkProgram( idProgram );
	glValidateProgram( idProgram );

	glClearColor( 1.0f, 1.0f, 1.0f, 1.0f ); //białe tło
	//glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // czarne tło
}

// ---------------------------------------
void CreateVertexShader( void )
{
	GLuint shader = glCreateShader( GL_VERTEX_SHADER );

	const GLchar * code =
	"#version 330 core\n"
    "layout(location = 0) in vec2 inPosition;"
    "void main() {"
    "   gl_Position = vec4(inPosition, 0.0, 1.0);"
    "}";

	glShaderSource( shader, 1, &code, NULL );
	glCompileShader( shader );

	GLint status;
	glGetShaderiv( shader, GL_COMPILE_STATUS, &status );
	if( status != GL_TRUE )
	{
		printf("Blad kompilacji vertex shadera!\n");
		exit(1);
	}
	glAttachShader( idProgram, shader );
}

// ---------------------------------------
void CreateFragmentShader( void )
{
	GLuint shader = glCreateShader( GL_FRAGMENT_SHADER );

	const GLchar *code =
	"#version 330 core\n"
    "out vec4 outColor;"
    "void main() {"
    "   // unikalny kolor na podstawie ID trójkąta\n"
    "   float r = fract(sin(float(gl_PrimitiveID) * 91.345) * 43758.5453);"
    "   float g = fract(sin(float(gl_PrimitiveID) * 37.567) * 12345.6789);"
    "   float b = fract(sin(float(gl_PrimitiveID) * 12.345) * 54321.9876);"
    "   // efekt zależny od pozycji na ekranie\n"
    "   vec2 pos = gl_FragCoord.xy / vec2(800.0, 800.0);\n"
    "   outColor = vec4(r, g, b, 1.0);\n"
    "}\n";


	glShaderSource( shader, 1, &code, NULL );
	glCompileShader( shader );

	GLint status;
	glGetShaderiv( shader, GL_COMPILE_STATUS, &status );
	if( status != GL_TRUE )
	{
		printf("Blad kompilacji fragment shadera!\n");
		exit(1);
	}
	glAttachShader( idProgram, shader );
}

// ---------------------------------------
void Reshape( int width, int height )
{
	glViewport( 0, 0, width, height );
}
