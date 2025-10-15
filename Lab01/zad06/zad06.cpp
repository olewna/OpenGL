#include <stdio.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>


// Identyfikatory obiektow
GLuint idProgram;	// potoku/programu
GLuint idVBO;		// bufora wierzcholkow
GLuint idVAO;		// tablic wierzcholkow
int numVertices;

const int WINDOW_SIZE = 800;

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
	glutInitWindowSize( WINDOW_SIZE, WINDOW_SIZE );
	glutCreateWindow( "zad06" );

	glutDisplayFunc( DisplayScene );
	glutReshapeFunc( Reshape );


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

	// Stworzenie obiektow OpenGLa
	// buforow, potoku, przekazanie danych
	// do pamieci karty graficznej
	Initialize();
	glutMainLoop();

	glDeleteProgram( idProgram );
	glDeleteBuffers( 1, &idVBO );
	glDeleteVertexArrays( 1, &idVAO );

	return 0;
}

// ---------------------------------------
void DisplayScene()
{
	// 1. czyszczenie bufora ramki koloru
	glClear( GL_COLOR_BUFFER_BIT );


	// 2. aktywowanie programu
	glUseProgram( idProgram );

    // 3. wlaczenie obiektu tablic wierzcholkow
    glBindVertexArray( idVAO );

    // 4. uruchomienie aktualnego programu na aktualnej tablicy wierzcholkow
    glDrawArrays( GL_TRIANGLE_FAN, 0, numVertices );

    GLint colorLoc = glGetUniformLocation(idProgram, "uOverrideColor");
    if(colorLoc != -1)
        glUniform3f(colorLoc, 1.0f, 0.0f, 0.0f);

    // 5. wylaczenie obiektu tablic wierzcholkow
    glLineWidth(1.0f);
    glDrawArrays(GL_LINE_LOOP, 1, numVertices - 1);

    glBindVertexArray( 0 );


	// 6. wylaczenie programu
	glUseProgram( 0 );


	// 7. wyswietlanie danych w oknie aplikacji
	glutSwapBuffers();
}

// ---------------------------------------
void Initialize()
{
	// Stworzenie identyfikatora obiektu oraz obiektu tablic wierzcholkow
	glGenVertexArrays( 1, &idVAO );
	glBindVertexArray( idVAO );

	// Generowanie identyfikatora obiektu oraz obiektu bufora
	glGenBuffers( 1, &idVBO );
	glBindBuffer( GL_ARRAY_BUFFER, idVBO );

	// --- generujemy wspó³rzêdne oœmiok¹ta ---
    const int N = 8;
    numVertices = N + 2; // œrodek + N + powrót
    float radius = 0.8f;

    GLfloat *vertices = new GLfloat[numVertices * 2];
    int i = 0;

    // œrodek
    vertices[i++] = 0.0f;
    vertices[i++] = 0.0f;

    // punkty na obwodzie
    for (int k = 0; k <= N; k++)
    {
        float angle = 2.0f * M_PI * k / N;
        vertices[i++] = radius * cos(angle);
        vertices[i++] = radius * sin(angle);
    }

	glBufferData(GL_ARRAY_BUFFER, numVertices * 2 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);
    delete[] vertices;

    glBindVertexArray(0);

	// ---------------------------------------
	// Etap (3) utworzenie potoku graficznego
	// ---------------------------------------

	idProgram = glCreateProgram();
	CreateVertexShader();
	CreateFragmentShader();

	// Walidacja programu
	glLinkProgram( idProgram );
	glValidateProgram( idProgram );


	// -----------------------------------------
	// Etap (4) ustawienie maszyny stanow OpenGL
	// -----------------------------------------

	// ustawianie koloru ktorym bedzie czyszczony bufor ramki
	glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
}

// ---------------------------------------
void CreateVertexShader( void )
{
	GLuint shader = glCreateShader( GL_VERTEX_SHADER );

	const GLchar * code =
	"#version 330 core\n"
    "layout(location = 0) in vec2 inPosition;"
    "out vec2 vPos;"
    "void main() {"
    "   vPos = inPosition;"
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
    "in vec2 vPos;"
    "out vec4 outColor;"
    "uniform vec3 uOverrideColor;"
    "void main() {"
    "   if(length(uOverrideColor) > 0.0) {"
    "      outColor = vec4(uOverrideColor, 1.0);"
    "   } else {"
    "       float d = length(vPos);"
    "       vec3 color = mix(vec3(1.0, 0.0, 0.0), vec3(0.0), smoothstep(0.0, 0.8, d));\n"
    "       outColor = vec4(color, 1.0);"
    "       }"
    "}";


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
