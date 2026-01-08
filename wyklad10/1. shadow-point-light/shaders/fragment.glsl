#version 150 core

in vec3 fragPos;
in vec3 fragNormal;

out vec4 outColor;

// Zmienne i parametry oswietlenia
uniform samplerCube tex_shadowCubeMap;

uniform float farPlane;		// tylna plaszczyzna bryly obcinania
uniform vec3  cameraPos;	// pozycja kamery
uniform vec3  lightPos;		// pozycja zrodla swiatla

// Oswietlenie
struct LightParam
{
    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;
    vec3 Position;
    float Attenuation;
};



// ---------------------------------------------------------------------------
// array of offset direction for sampling
// https://learnopengl.com/Advanced-Lighting/Shadows/Point-Shadows
// ---------------------------------------------------------------------------
vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1),
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);


// ---------------------------------------------------------------------------
// Reference: https://learnopengl.com/Advanced-Lighting/Shadows/Point-Shadows
// ---------------------------------------------------------------------------

float calcPointShadow(vec3 lightPosition, vec3 fragPos, float farPlane)
{
	// get vector between fragment position and light position
	vec3 fragToLight = fragPos - lightPosition;

	// now get current linear depth as the length between the fragment and light position
	float currentDepth = length(fragToLight);

	// use the light to fragment vector to sample from the depth map
	float closestDepth = texture(tex_shadowCubeMap, fragToLight).r * farPlane;


	// return (currentDepth > closestDepth) ? 1.0 : 0.0;


	// simple test
	vec3 lightDirection = normalize(fragPos - lightPosition);

	float bias = 0.01;
	float shadow = 0.0;

	bias = max(0.04 * (1.0 - dot(fragNormal, lightDirection)), 0.01);

	shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;
	return shadow;


	
	// filtering
	// int samples = 20;
	// float viewDistance = length(cameraPos - fragPos.xyz);
	// float diskRadius = (1.0 + (viewDistance / farPlane)) / 25.0;

	// for(int i = 0; i < samples; ++i)
	// {
	// 	float closestDepth = texture(tex_shadowCubeMap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
	// 	closestDepth *= farPlane;   // undo mapping [0;1]

	// 	if(currentDepth - bias > closestDepth)
	// 		shadow += 1.0;
	// }

	// shadow /= float(samples);

	// return shadow;
}



// ---------------------------------------------------------------------------
// UWAGA: Oswietlenie punktowe zwraca diffuse+specular, bez ambientu
// ---------------------------------------------------------------------------
vec3 calcPointLight(LightParam light, vec3 cameraPos, vec3 fragPos, vec3 fragNormal)
{

	// Distance light
	float Light_dist = length(light.Position - fragPos);
	float Attenuation = 1.0 / (1.0 + light.Attenuation * (Light_dist));

	// ---------------------------
	// Ambient
	// vec4 ambientPart = myLight.Ambient * Attenuation;
	// Ambient policzymy majac wspolczynnik cienia
	// ---------------------------
	// Diffuse

	// Obliczenie wektora (swiatlo - wierzcholek)
	// czyli kierunku padania swiatla na wierzcholek
	vec3 lightDir = normalize(light.Position - fragPos);

	// obliczenie kata pomiedzy wektorem lightDir oraz wektorem normalnym
	// wartosc kata okresla pod jakim katem padaja promienie
	float lightCoeff = max(dot(fragNormal, lightDir), 0.0);
	vec3 diffusePart = lightCoeff * light.Diffuse * Attenuation;


	// ------------------
	// Specular
	vec3 viewDir = normalize(cameraPos - fragPos);
	vec3  reflectDir = reflect(-lightDir, fragNormal);
	// obliczanie wspolczynnika specular z parametrem shininess
	float specularCoeff = pow(max(dot(viewDir, reflectDir), 0.0), 64);
	vec3  specularPart = specularCoeff * light.Specular * Attenuation;

	// -----------------
	// Ostateczny
	return (diffusePart + specularPart);
}



// ---------------------------------------------------------------------------
void main()
{

	// Przykladowe parametry oswietlenia
	LightParam myLight = LightParam(
		vec3(0.2, 0.2, 0.2),
		vec3(0.8, 0.8, 0.8),
		vec3(0.5, 0.5, 0.5),
		lightPos,
		0.01
	);


	// Kolor wyjsciowy
	vec3 fragColor = vec3(0.1, 1.0, 0.1);

	// Oswietlenie punktowe
	vec3 diffSpecLight = calcPointLight(myLight, cameraPos, fragPos, fragNormal);
	vec3 ambientLight = myLight.Ambient;

	// Cienie
	float shadowCoef = calcPointShadow(myLight.Position, fragPos, farPlane);

	// Kolor finalny
	vec3  finalColor = (ambientLight + (1-shadowCoef)*diffSpecLight) * fragColor;

	outColor = vec4(finalColor, 1.0);
}
