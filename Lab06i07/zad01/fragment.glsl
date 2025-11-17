#version 330 core

in vec3 inPosition;
in vec2 TexCoord;
in vec3 inNormal;

out vec4 FragColor;


uniform sampler2D tex0;
uniform vec3 cameraPos;
uniform int shadingModel;
uniform bool uLightingEnabled;
uniform int uDrawLightSphere;

// Struktura parametrow swiatla
struct LightParam
{
	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;
	vec3 Attenuation;
	vec3 Position; // Direction dla kierunkowego
};
// Przykladowe swiatlo
uniform LightParam myLight;

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

void main()
{
    vec3 baseColor = texture(tex0, TexCoord).rgb;

	if (uDrawLightSphere == 1) {
		FragColor = vec4(myLight.Diffuse, 1.0);   // kolor światła
		return;
	}

    if (!uLightingEnabled) {
        FragColor = vec4(baseColor, 1.0);
        return;
    }


    vec3 lightColor = calculatePointLight(myMaterial, myLight);

    vec3 result = lightColor * baseColor;

    FragColor = vec4(result, 1.0);
}
