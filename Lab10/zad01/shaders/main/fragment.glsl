#version 330 core

#define MAX_LIGHTS 5

in vec3 inPosition;
in vec2 TexCoord;
in vec3 inNormal;

out vec4 FragColor;

uniform sampler2D tex0; // tekstury obiektow na scene
uniform sampler2D texture_shadowMap; // mapa cieni

in vec4 fragPosLight;

uniform vec3 cameraPos;
uniform int shadingModel;
uniform bool uLightingEnabled;
uniform int uDrawLightSphere;
uniform int lightMode;

uniform int showShadows;
uniform float maxBiasShadow;
uniform float minBiasShadow;

// Struktura parametrow swiatla
struct LightParam{
	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;
	vec3 Attenuation;
	vec3 Position; // Direction dla kierunkowego
	vec3 Direction;
};

// Struktura parametrow materialu
struct MaterialParam{
	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;
	float Shininess;
};

// Przykladowy material
uniform MaterialParam myMaterial;
uniform int activeLights;

// światła
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

// Shadow calculation
float ShadowCalculation(vec4 fragPosLightSpace)
{
    if (showShadows == 0) {
        return 0.0;
    }

    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords*0.5 + 0.5;

    // if(projCoords.z > 1.0) return 0.0;

    float closestDepth = texture(texture_shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    float bias = max(maxBiasShadow*(1.0-dot(inNormal,normalize(lights[0].Direction))), minBiasShadow);

    float shadow = (currentDepth - bias > closestDepth) ? 1.0 : 0.0;

    // vec4 texColor = texture(tex0, TexCoord);
    // if(texColor.a < 0.5) shadow = 0.0;

    return shadow;
}

void main()
{
    vec3 baseColor = texture(tex0, TexCoord).rgb;

	if (uDrawLightSphere == 1) {
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

    vec3 result;

    // cień
    if (lightMode == 1) {
        vec4 fragPosLightSpace = fragPosLight;
        float shadow = ShadowCalculation(fragPosLightSpace);

        result = (lights[0].Ambient + (1.0 - shadow) * lightColor) * baseColor;
    } else {
        result = lightColor * baseColor;
    }

    FragColor = vec4(result, 1.0);
}
