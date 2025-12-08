#version 330 core

#define MAX_LIGHTS 8

in vec3 inPosition;
in vec2 TexCoord;
in vec3 inNormal;

out vec4 FragColor;

uniform sampler2D tex0;
uniform vec3 cameraPos;
uniform int shadingModel;
uniform bool uLightingEnabled;
uniform int uDrawLightSphere;

uniform int lightMode;

// Struktura parametrow swiatla
struct LightParam
{
	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;
	vec3 Attenuation;
	vec3 Position; // Direction dla kierunkowego
	vec3 Direction;
};
// Przykladowe swiatlo
// uniform LightParam myLight;

// Struktura parametrow materialu
struct MaterialParam
{
	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;
	float Shininess;
};
// Przykladowy material
uniform MaterialParam myMaterial;
uniform int activeLights;
uniform LightParam lights[MAX_LIGHTS];
uniform int lightIndexToDraw;

vec3 calculatePointLight(MaterialParam myMaterial, LightParam myLight)
{
	// Skladowa tlumienia
	float LV = length( myLight.Position - inPosition.xyz );
	float Latt = 1.0/( myLight.Attenuation.x + myLight.Attenuation.y * LV + myLight.Attenuation.z * LV * LV );

	// Ambient part
	vec3 ambient = myLight.Ambient * myMaterial.Ambient;

	// Diffuse part
	vec3 L = normalize(myLight.Position - inPosition.xyz);
	vec3 diffuse = max( dot(L, inNormal ), 0 ) * myLight.Diffuse * myMaterial.Diffuse;

	// Specular part
	vec3 E = normalize(cameraPos - inPosition.xyz);

    float specCoef;

    if (shadingModel == 1) {
        // -------- BLINN-PHONG --------
        vec3 H = normalize(L + E);
	    specCoef = pow(max(dot(H, inNormal), 0.0), myMaterial.Shininess);
    } else if (shadingModel == 0) {
        // -------- PHONG --------
        vec3 R = reflect(-L, inNormal);
        specCoef = pow(max(dot(E, R), 0.0), myMaterial.Shininess);
    }
	
	vec3 specular = specCoef * myLight.Specular * myMaterial.Specular;

	vec3 lightCoef = ambient + Latt * ( diffuse + specular);

	return lightCoef;
}

vec3 calculateDirectionalLight(MaterialParam myMaterial, LightParam myLight)
{
    vec3 L = normalize(-myLight.Direction);   // kierunek padania
    vec3 N = normalize(inNormal);
    vec3 E = normalize(cameraPos - inPosition.xyz);

    // Ambient
    vec3 ambient = myLight.Ambient * myMaterial.Ambient;

    // Diffuse
    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = diff * myLight.Diffuse * myMaterial.Diffuse;

    // Specular
    float specCoef;

    if (shadingModel == 1) {
        // Blinn-Phong
        vec3 H = normalize(L + E);
        specCoef = pow(max(dot(N, H), 0.0), myMaterial.Shininess);
    }
    else {
        // Phong
        vec3 R = reflect(-L, N);
        specCoef = pow(max(dot(E, R), 0.0), myMaterial.Shininess);
    }

    vec3 specular = specCoef * myLight.Specular * myMaterial.Specular;

    return ambient + diffuse + specular;
}

void main()
{
    vec3 baseColor = texture(tex0, TexCoord).rgb;

	if (uDrawLightSphere == 1) {
        // for (int i = 0; i < activeLights; i++) {
		//     lightColor = calculatePointLight(myMaterial, light[i]);
        // }
		FragColor = vec4(lights[lightIndexToDraw].Diffuse, 1.0);   // kolor światła
		return;
	}

    if (!uLightingEnabled) {
        FragColor = vec4(baseColor, 1.0);
        return;
    }

    vec3 lightColor = vec3(0);

	if (lightMode == 0) {
        for (int i = 0; i < activeLights; i++) {
		    lightColor += calculatePointLight(myMaterial, lights[i]);
        }
	} else if (lightMode == 1) {
        LightParam myLight = lights[0];
		lightColor = calculateDirectionalLight(myMaterial, myLight);
	}

    vec3 result = lightColor * baseColor;

    FragColor = vec4(result, 1.0);
}
