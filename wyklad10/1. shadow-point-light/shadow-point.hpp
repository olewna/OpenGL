#ifndef __SHADOW_POINT_LIGHT
#define __SHADOW_POINT_LIGHT

// ---------------------------------------
class CShadowPointLight
{

public:

	// Polozenie swiatla
	glm::vec3 lightPosition;

	// Identyfikatory obiektow
	GLuint idProgram;			// program generujacy mape cieni
	GLuint idTextureCube;		// tekstura
	GLuint idFrameBuffer[6];	// 6 x FBO


	// Rozmiar mapy cieni
	int DepthMap_Width = 1024;
	int DepthMap_Height = 1024;

	// Bryla obcinania (frustum)
	float frustumNear = 1.0f;
	float frustumFar = 20.0f;
	// macierz projection oraz view
	// do renderingu kazdej z szesciu scian
	glm::mat4 matProj;
	glm::mat4 matViews[6];


	// ----------------------------------------------------------
	// Etap 1. Inicjalizacja calego procesu generowania
	// cieni z podaniem punktu polozenia zrodla swiatla
	void Init(glm::vec3 _pos);

	// Aktualizacja polozenia oswietlenia i macierzy rzutowania
	// potrzebna podczas inicjalizacji oraz zawsze wtedy, kiedy
	// dokonujemy zmiany polozenia oswietlenia
	void UpdateViewMat();

	// Etap 2: Generowanie mapy cieni
	// pomiedzy tymi funkcjami renderujemy to co ma rzucac cien
	// i byc uwzglednione w mapie cieni
	// UWAGA! generowanie obiektow powinno odbywac sie za pomoca
	// programu idProgram z tego obiektu
	void GenBegin(int face);
	void GenEnd(void);


	// Etap 3: Podczas finalnego renderingu na ekran
	// Wysylanie do programu _prog wszystkich potrzebnych uniformow
	// Ustawianie tekstury mapy cieni na slot _texSlot
	void SendUniforms(GLuint _prog, GLuint _texSlot, glm::vec3 _camPos);

	// Czyszczenie
	void Clean();

};


// -------------------------------------------------------
void CShadowPointLight::SendUniforms(GLuint _prog, GLuint _texSlot, glm::vec3 _camPos)
{

	// Shadow map textura na slot _texSlot
	glActiveTexture(GL_TEXTURE0 + _texSlot);
	glBindTexture(GL_TEXTURE_CUBE_MAP, idTextureCube);
	glUniform1i(glGetUniformLocation( _prog, "tex_shadowCubeMap" ), _texSlot);

	// Parametry oswietlenia
	glUniform3fv( glGetUniformLocation( _prog, "lightPos" ), 1, glm::value_ptr(lightPosition) );
	glUniform1f( glGetUniformLocation( _prog, "farPlane" ), frustumFar );

	// Inne : pozycja kamery
	glUniform3fv( glGetUniformLocation( _prog, "cameraPos" ), 1, glm::value_ptr(_camPos) );


}

// -------------------------------------------------------
void CShadowPointLight::GenBegin(int face)
{

	glBindFramebuffer(GL_FRAMEBUFFER, idFrameBuffer[face]);
	glViewport(0, 0, DepthMap_Width, DepthMap_Height);
	glClear(GL_DEPTH_BUFFER_BIT);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X+face, idTextureCube, 0);

	// AKTYWUJEMY program
	glUseProgram( idProgram );

	// Wysylanie macierzy
	glUniformMatrix4fv( glGetUniformLocation( idProgram, "matProj" ), 1, GL_FALSE, glm::value_ptr(this->matProj));
	glUniformMatrix4fv( glGetUniformLocation( idProgram, "matView" ), 1, GL_FALSE, glm::value_ptr(this->matViews[face]));
	glUniform3fv( glGetUniformLocation( idProgram, "lightPos" ), 1, glm::value_ptr(lightPosition) );
	glUniform1f( glGetUniformLocation( idProgram, "farPlane" ), frustumFar );

}


// -------------------------------------------------------
void CShadowPointLight::GenEnd(void)
{

	// WYLACZAMY program, przechodzimy do domyslnego FBO
	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


// -------------------------------------------------------
void CShadowPointLight::Init(glm::vec3 _pos)
{
	// 1. Pozycja zrodla swiatla
	this->lightPosition = _pos;

	// 2. Textura do depthMapy
	glGenTextures(1, &idTextureCube);
	glBindTexture(GL_TEXTURE_CUBE_MAP, idTextureCube);

	for (int i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, DepthMap_Width, DepthMap_Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	// 3. 6 x FBO z buforem glebokosci na kazda sciane
	for (int i=0; i < 6; i++)
	{
		glGenFramebuffers(1, &idFrameBuffer[i]);
		glBindFramebuffer(GL_FRAMEBUFFER, idFrameBuffer[i]);
		//glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, idTextureCube, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, idTextureCube, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			printf("Error: Framebuffer %d is not complete!\n", i);
			exit(1);
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// 4. Potok do generowania samych cieni
	idProgram = glCreateProgram();
	glAttachShader( idProgram, LoadShader(GL_VERTEX_SHADER, "shaders/depthmap-cube-vertex.glsl"));
	glAttachShader( idProgram, LoadShader(GL_FRAGMENT_SHADER, "shaders/depthmap-cube-fragment.glsl"));
	LinkAndValidateProgram( idProgram );

	// 5. Ustawienia macierzy rzutowania kamery oswietlenia
	matProj = glm::perspective(glm::radians(90.0f), DepthMap_Width/(float)DepthMap_Height, frustumNear, frustumFar);

	// aktualizacja macierzy view
	UpdateViewMat();

}

// -------------------------------------------------------
// Aktualizacja tablicy 6 macierzy view
void CShadowPointLight::UpdateViewMat()
{
	// Obliczanie nowych macierzy widoku
	matViews[0] = glm::lookAt(lightPosition , lightPosition + glm::vec3( 1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0));
	matViews[1] = glm::lookAt(lightPosition , lightPosition + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0));
	matViews[2] = glm::lookAt(lightPosition , lightPosition + glm::vec3( 0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0));
	matViews[3] = glm::lookAt(lightPosition , lightPosition + glm::vec3( 0.0,-1.0, 0.0), glm::vec3(0.0, 0.0,-1.0));
	matViews[4] = glm::lookAt(lightPosition , lightPosition + glm::vec3( 0.0, 0.0, 1.0), glm::vec3(0.0,-1.0, 0.0));
	matViews[5] = glm::lookAt(lightPosition , lightPosition + glm::vec3( 0.0, 0.0,-1.0), glm::vec3(0.0,-1.0, 0.0));
}

// -------------------------------------------------------
// Usuwanie obiektow OpenGLa
void CShadowPointLight::Clean()
{
	glDeleteProgram(idProgram);
	glDeleteTextures(1, &idTextureCube);
	glDeleteFramebuffers(6, &idFrameBuffer[0]);
}

#endif
