#include <GL/glew.h>
#include <GL/freeglut.h>
#include <string>
#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "utilities.hpp"
#include "objloader.hpp"

void UpdateCamera();

// --------------------------------------------------------------
// 1. Klasa do obslugi potoku
// --------------------------------------------------------------
class CProgram
{

public:
    CProgram() : idProgram(0) {}
    ~CProgram() { Clean(); }

	bool Init() {
        idProgram = glCreateProgram();
        return idProgram != 0;
	}

	void Clean() {
        if (idProgram) {
            glDeleteProgram(idProgram);
            idProgram = 0;
        }
	}


	bool LoadShaders(const char* vertexFile, const char* fragmentFile) {
        glAttachShader(idProgram, LoadShader(GL_VERTEX_SHADER, vertexFile));
        glAttachShader(idProgram, LoadShader(GL_FRAGMENT_SHADER, fragmentFile));
        LinkAndValidateProgram( idProgram );

        return true;
	}


	void Use() {
        glUseProgram(idProgram);
	}

	void UnUse() {
        glUseProgram(0);
	}


    // uniformy
	void SetFloat(const char* name, float value)
    {
        glUniform1f(glGetUniformLocation(idProgram, name), value);
    }

    void SetInt(const char* name, int value)
    {
        glUniform1i(glGetUniformLocation(idProgram, name), value);
    }

    void SetMat4(const char* name, const glm::mat4& matrix)
    {
        glUniformMatrix4fv(glGetUniformLocation(idProgram, name), 1, GL_FALSE, glm::value_ptr(matrix));
    }


    // gettery i settery
	GLuint GetId() const { return idProgram; }


private:

	GLuint idProgram;

};

// --------------------------------------------------------------
// 2. Oraz druga klasa do obslugi obiektow 3D
// --------------------------------------------------------------
class CMesh
{

public:
    CMesh() : idVAO(0), idVBO_coords(0), idVBO_uvs(0), idVBO_normals(0), vertexCount(0), matModel(1.0f) {}
    ~CMesh() { Clean(); }

	// Metody tworzaca VAO i VBO z pliku OBJ
	bool CreateFromOBJ(const char* filename) {
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec2> uvs;
        std::vector<glm::vec3> normals;

        if (!loadOBJ(filename, vertices, uvs, normals))
        {
            std::cerr << "Nie udało się wczytać pliku OBJ: " << filename << std::endl;
            return false;
        }

        vertexCount = vertices.size();

        // tworzenie VAO i VBO
        glGenVertexArrays(1, &idVAO);
        glBindVertexArray(idVAO);

        // VBO dla coordsów
        glGenBuffers(1, &idVBO_coords);
        glBindBuffer(GL_ARRAY_BUFFER, idVBO_coords);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        // VBO dla uvs
        if (!uvs.empty())
        {
            glGenBuffers(1, &idVBO_uvs);
            glBindBuffer(GL_ARRAY_BUFFER, idVBO_uvs);
            glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), uvs.data(), GL_STATIC_DRAW);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
        }

        // VBO dla normalsów
        if (!normals.empty())
        {
            glGenBuffers(1, &idVBO_normals);
            glBindBuffer(GL_ARRAY_BUFFER, idVBO_normals);
            glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        }

        glBindVertexArray(0);

        return true;
	}

	// Metoda czyszczaca, ktora posprzata
	// usunie pamiec, obiekty VAO itd.
	void Clean() {
        if (idVBO_coords)  glDeleteBuffers(1, &idVBO_coords);
        if (idVBO_uvs)     glDeleteBuffers(1, &idVBO_uvs);
        if (idVBO_normals) glDeleteBuffers(1, &idVBO_normals);
        if (idVAO)         glDeleteVertexArrays(1, &idVAO);

        idVAO = idVBO_coords = idVBO_uvs = idVBO_normals = 0;
    }


	// Oczywiscie metoda generujaca obraz na ekranie
	// To ona wywola metode glDrawArrays()
	void Draw() {
        glBindVertexArray(idVAO);
        glDrawArrays(GL_TRIANGLES, 0, vertexCount);
        glBindVertexArray(0);
	}

	void SetPosition(const glm::vec3& pos) {
        matModel = glm::translate(glm::mat4(1.0f), pos);
    }

    void SetRotation(const glm::vec3& angles) {
        matModel = glm::rotate(matModel, glm::radians(angles.x), glm::vec3(1,0,0));
        matModel = glm::rotate(matModel, glm::radians(angles.y), glm::vec3(0,1,0));
        matModel = glm::rotate(matModel, glm::radians(angles.z), glm::vec3(0,0,1));
    }

    glm::mat4 GetModelMatrix() const { return matModel; }


private:

	// Skladowe

	// Identyfikator VAO
	GLuint idVAO;

	// Identyfikatory buforow na potrzeby VAO
	GLuint idVBO_coords;
	GLuint idVBO_uvs;
    GLuint idVBO_normals;


	// Macierz modelu, to w niej bedzie
	// przechowana pozycja i orientacja obiektu
	// na scenie
	size_t vertexCount;
	glm::mat4 matModel;
};


// --------------------------------------------------------------
// PRZYKLAD UZYCIA
// --------------------------------------------------------------

// Majac takie klasy bedzie nam wygodnie tworzyc obiekty potokow
// oraz obiekty.

// Przykladowo:  majac obiekt potoku oraz obiekty 3D
// jako zmienne globalne inicjalizujemy je w Initialize
// a nastepnie uzywamy w DisplayScene.


CProgram glownyProgram;
CMesh kaktus3D, monkey3D, ptaku3D;

glm::mat4 matProj, matView;

// ---------------------------------------
void DisplayScene()
{
	// Czyszczenie ramki
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );


	// Geometria sceny i obliczanie macierzy rzutowania
	glm::mat4 matPV = matProj * matView;


	// Aktywujemy potok
	glownyProgram.Use();

		// Przekazujemy zmienne jednorodne
		// na przykald macierze projection i view
		// tutaj: jako jedna (wymnozona juz)
    glownyProgram.SetMat4("matPV", matPV);

    glownyProgram.SetMat4("matModel", kaktus3D.GetModelMatrix());
    kaktus3D.Draw();

    glownyProgram.SetMat4("matModel", monkey3D.GetModelMatrix());
    monkey3D.Draw();

    glownyProgram.SetMat4("matModel", ptaku3D.GetModelMatrix());
    ptaku3D.Draw();


	// Dezaktywujemy potok
	glownyProgram.UnUse();
	glutSwapBuffers();
}

// ---------------------------------------------------
void Initialize()
{
    glewInit();
    glEnable(GL_DEPTH_TEST);
    glClearColor(1, 1, 1, 1.0f);

	// Tworzenie obiektu potoku
	// UWAGA: Chociaz zamiast dwoch metod, moze wygodniej
	// byloby stworzyc tylko jedna?
	glownyProgram.Init();
	glownyProgram.LoadShaders("vertex.glsl", "fragment.glsl");

	// Tworzenie obiektow 3D z plikow OBJ
	kaktus3D.CreateFromOBJ("kaktus.obj");
	kaktus3D.SetPosition(glm::vec3(0.0, 0.0, -2.0));

	monkey3D.CreateFromOBJ("monkey.obj");
	monkey3D.SetPosition(glm::vec3(3.0, 0.0, -1.0));

	ptaku3D.CreateFromOBJ("koliber.obj");
	ptaku3D.SetPosition(glm::vec3(-3.0, 0.0, -1.0));

	// itd...

    matProj = glm::perspective(glm::radians(60.0f), 800.0f/600.0f, 0.1f, 100.0f);
    matView = glm::lookAt(
        glm::vec3(0.0f, 0.0f, 8.0f),  // pozycja kamery
        glm::vec3(0.0f, 0.0f, 0.0f),  // punkt, na który patrzymy
        glm::vec3(0.0f, 1.0f, 0.0f)   // wektor „up”
    );
    UpdateCamera();
}

glm::vec3 camTarget(0.0f, 0.0f, 0.0f);  // punkt, na który patrzymy
float camDistance = 10.0f;              // odległość od środka sceny
float camAngleX = 20.0f;                // obrót w pionie (stopnie)
float camAngleY = 0.0f;

void UpdateCamera()
{
    // Przelicz pozycję kamery na podstawie kąta i odległości
    float radX = glm::radians(camAngleX);
    float radY = glm::radians(camAngleY);

    float x = camDistance * sin(radY) * cos(radX);
    float y = camDistance * sin(radX);
    float z = camDistance * cos(radY) * cos(radX);

    glm::vec3 camPos = camTarget + glm::vec3(x, y, z);

    matView = glm::lookAt(camPos, camTarget, glm::vec3(0,1,0));
}


void Keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 27: // ESC
            exit(0);
            break;

        case 'w': camAngleX += 2.0f; break;   // obrót w pionie w górę
        case 's': camAngleX -= 2.0f; break;   // w dół
        case 'a': camAngleY -= 2.0f; break;   // obrót w lewo
        case 'd': camAngleY += 2.0f; break;   // obrót w prawo

        case 'e': camDistance -= 0.5f; if(camDistance<1.0f) camDistance=1.0f; break; // przybliżanie
        case 'q': camDistance += 0.5f; break; // oddalanie
    }

    UpdateCamera();
    glutPostRedisplay();
}


int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("lab04zad01");

    Initialize();
    glutKeyboardFunc( Keyboard );
    glutDisplayFunc(DisplayScene);
    glutMainLoop();

    return 0;
}
// Majac takie klasy bedzie nam duzo wygodniej w przyszlosci.
// O szczegoly pytaj prowadzacego i kolegow/kolezanki.
