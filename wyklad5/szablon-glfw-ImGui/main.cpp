// -------------------------------------------------
// Programowanie grafiki 3D w OpenGL / UG
// ------------------------------------------------
// Szablon aplikacji w OpenGL+GLFW+ImGui
//
// ImGui:
// https://github.com/ocornut/imgui
//
// Kompilacja pod windowsem
// g++ *.cpp glad.c ImGui\*.cpp -lglfw3 -lopengl32 -lgdi32
//
// Wymagana jest bibliteka glad
// https://glad.dav1d.de/
//
// Dolaczony plik glad.c oraz naglowki
// zostaly stworzone pod Opengl 4.5 i nalezy je
// podmienic w razie problemow.
// ------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// Biblioteki GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


// Macierze transformacji i rzutowania
glm::mat4 matProj;
glm::mat4 matView;
glm::mat4 matModel;

// Parametry okna aplikacji
GLFWwindow* window = nullptr;
int windowWidth = 800;
int windowHeight = 600;

// Zmienna czas do animacji
float Time = 0.0;

#include "myImGui.hpp"
#include "utilities.hpp"
#include "objloader.hpp"


// Wektory na dane z pliku OBJ
std::vector<glm::vec3> OBJ_vertices;
std::vector<glm::vec2> OBJ_uvs;
std::vector<glm::vec3> OBJ_normals;


// Identyfikatory obiektow OpenGLa
GLuint idProgram;	// programu
GLuint idVBO;		// bufora wierzcholkow
GLuint idVAO;		// tablic wierzcholkow
GLuint idVBO_coord; // bufory
GLuint idVBO_color; // bufory



// ---------------------------------------
void DisplayScene()
{
    if (show_animation) Time += 0.01;

    // Czyszczenie bufora koloru i glebokosci
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// Aktualizacja macierzy widoku (obsluga myszy)
	matView = UpdateViewMatrix();


	// Aktywowanie potoku
	glUseProgram( idProgram );


		// Przekazanie macierzy
		glUniformMatrix4fv( glGetUniformLocation(idProgram, "matProj"), 1, GL_FALSE, glm::value_ptr(matProj));
		glUniformMatrix4fv( glGetUniformLocation(idProgram, "matView"), 1, GL_FALSE, glm::value_ptr(matView));


		// Obiekt z pliku OBJ
		matModel = glm::mat4(1.0);
		matModel = glm::rotate(matModel, Time, glm::vec3(0,1,0));
		glUniformMatrix4fv( glGetUniformLocation(idProgram, "matModel"), 1, GL_FALSE, glm::value_ptr(matModel));

		// Rendering
		glBindVertexArray( idVAO );
		glDrawArrays( GL_TRIANGLES, 0, OBJ_vertices.size() );
		glBindVertexArray( 0 );


	// Wylaczanie
	glUseProgram( 0 );

}

// ---------------------------------------
void Initialize()
{
    // Pierwsze ustalanie macierzy rzytowania
    if (windowHeight != 0)
    matProj = glm::perspective(glm::radians(80.0f), windowWidth/(float)windowHeight, 0.1f, 50.0f);


    // Wczytanie pliku obj
    if (!loadOBJ("assets/scene.obj", OBJ_vertices, OBJ_uvs, OBJ_normals))
    {
        printf("Not loaded!\n");
        exit(1);
    }

    // Potok
    idProgram = glCreateProgram();
    glAttachShader( idProgram, LoadShader(GL_VERTEX_SHADER, "shaders/vertex.glsl"));
    glAttachShader( idProgram, LoadShader(GL_FRAGMENT_SHADER, "shaders/fragment.glsl"));
    LinkAndValidateProgram( idProgram );

    // VAO
    glGenVertexArrays( 1, &idVAO );
    glBindVertexArray( idVAO );
        // Bufor na wspolrzedne wierzcholkow
        glGenBuffers( 1, &idVBO_coord );
        glBindBuffer( GL_ARRAY_BUFFER, idVBO_coord );
        glBufferData( GL_ARRAY_BUFFER, OBJ_vertices.size() * sizeof(glm::vec3), &OBJ_vertices[0], GL_STATIC_DRAW );
        glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
        glEnableVertexAttribArray( 0 );
        // Bufor na kolory
        glGenBuffers( 1, &idVBO_color );
        glBindBuffer( GL_ARRAY_BUFFER, idVBO_color );
        glBufferData( GL_ARRAY_BUFFER, OBJ_normals.size() * sizeof(glm::vec3), &OBJ_normals[0], GL_STATIC_DRAW );
        glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, NULL );
        glEnableVertexAttribArray( 1 );
    glBindVertexArray( 0 );

    // Test glebokosci
    glEnable(GL_DEPTH_TEST);

}


// ---------------------------------------------------
int main( int argc, char *argv[] )
{
	// Funkcja zwrotna do bledow
	glfwSetErrorCallback(error_callback);
	// Inicjalizacja glfw
	if (!glfwInit()) exit(EXIT_FAILURE);
	// Ustalenie wersji OpenGL na 4.5
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    // -----------------------------------
    // ImGui ImGui ImGui ImGui ImGui ImGui
    // -----------------------------------
    float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor());


    // Okno aplikacji i kontekst
    window = glfwCreateWindow((int)(windowWidth * main_scale), (int)(windowHeight * main_scale), "Szablon OpenGL3+glfw+ImGui", nullptr, nullptr);
    if (window == nullptr) return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // vsync
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Failed to initialize GLAD\n");
        return -1;
    }

    // Funkcje zwrotne
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // -----------------------------------
    // ImGui ImGui ImGui ImGui ImGui ImGui
    // -----------------------------------
    ImGui_Init(window);


	// Inicjalizacja
	Initialize();


    // Glowna petla
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

        // Render Sceny
        DisplayScene();

        // -----------------------------------
        // ImGui ImGui ImGui ImGui ImGui ImGui
        // -----------------------------------
        ImGui_Display();


		glfwSwapBuffers(window);
	}

    // -----------------------------------
    // ImGui ImGui ImGui ImGui ImGui ImGui
    // -----------------------------------
	ImGui_Clean();


	// Czyszczenie
    glDeleteVertexArrays(1, &idVAO);
    glDeleteBuffers(1, &idVBO_coord);
    glDeleteBuffers(1, &idVBO_color);
    glDeleteProgram(idProgram);


    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);

	return 0;
}
