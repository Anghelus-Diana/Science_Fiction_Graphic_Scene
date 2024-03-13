#version 410 core

in vec3 fPosition;
in vec3 fNormal;
in vec2 fTexCoords;

out vec4 fColor;

//matrices
uniform mat4 model;
uniform mat4 view;
uniform mat3 normalMatrix;

//lighting
uniform vec3 lightDir;
uniform vec3 lightColor;




// textures
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;

//components
vec3 ambient;
float ambientStrength = 0.2f;
vec3 diffuse;
vec3 specular;
float specularStrength = 0.5f;

//fog
uniform int activateFog;
vec3 finalColor = vec3(0.0f);

//PointLight
struct Material {
   float shininess;
};

struct PointLight{
    vec3 position;
    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

//SpotLight 
uniform int activateSpotLight;

float spotQuadratic = 0.02f;
float spotLinear = 0.09f;
float spotConstant = 1.0f;

vec3 spotLightAmbient = vec3(0.0f, 0.0f, 0.0f);
vec3 spotLightSpecular = vec3(1.0f, 1.0f, 1.0f);
vec3 spotLightColor = vec3(12,12,12);

uniform float spotlight1;
uniform float spotlight2;

uniform vec3 spotLightDirection;
uniform vec3 spotLightPosition;


//materials
uniform Material material;

//Light 
uniform PointLight pointLight;


//Control vraibles light
uniform int activatePointLight;



void computeDirLight()
{
    //compute eye space coordinates
    vec4 fPosEye = view * model * vec4(fPosition, 1.0f);
    vec3 normalEye = normalize(normalMatrix * fNormal);

    //normalize light direction
    vec3 lightDirN = vec3(normalize(view * vec4(lightDir, 0.0f)));

    //compute view direction (in eye coordinates, the viewer is situated at the origin
    vec3 viewDir = normalize(- fPosEye.xyz);

    //compute ambient light
    ambient = ambientStrength * lightColor;

    //compute diffuse light
    diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;

    //compute specular light
    vec3 reflectDir = reflect(-lightDirN, normalEye);
    float specCoeff = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
    specular = specularStrength * specCoeff * lightColor;
}

vec3 computePointLight(PointLight light, sampler2D diffuseTexture, sampler2D specularTexture)
{
    //compute eye space coordinates
    vec4 fPosEye = vec4(fPosition, 1.0f);
    vec3 normalEye = normalize(fNormal);

    //compute ambient light
    vec3 ambient = vec3(0.0f,1.0f, 0.0f)*light.ambient * texture(diffuseTexture, fTexCoords).rgb;

    //compute diffuse light
    vec3 lightDirN = normalize(light.position - fPosEye.xyz);
    float diffCoeff = max(dot(normalEye, lightDirN), 0.0f);
	vec3 diffuse = vec3(0.0f,1.0f, 0.0f)*light.diffuse * (diffCoeff * texture(diffuseTexture, fTexCoords).rgb);

    //compute specular light
    vec3 viewDirN = normalize(light.position - fPosEye.xyz);
    vec3 halfVector = normalize(lightDirN + viewDirN);
    float specCoeff = pow(max(dot(normalEye, halfVector), 0.0f), material.shininess);
    vec3 specular = vec3(0.0f,1.0f, 0.0f)*light.specular * (specCoeff * texture(specularTexture, fTexCoords).rgb);

    //calcualte distance from light to fragment and attenation
	float distance = length(light.position - fPosEye.xyz);
	float attenation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    //attenuate each component
    ambient *=  10*attenation;
    diffuse *= 10*attenation;
    specular *= 10*attenation;

    return min((ambient + diffuse + specular), 1.0f);
}

vec3 computeLightSpotComponents() {
	vec3 cameraPosEye = vec3(0.0f);
    vec4 fPosEye = vec4(fPosition, 1.0f);
	vec3 lightDir = normalize(spotLightPosition - fPosEye.xyz);
	vec3 normalEye = normalize(normalMatrix * fNormal);
    vec3 lightDirN = vec3(normalize(view * vec4(lightDir,0.0f)));
	vec3 viewDirN = normalize(cameraPosEye - fPosEye.xyz);
	vec3 halfVector = normalize(lightDirN + viewDirN);

	float diff = max(dot(fNormal, lightDir), 0.0f);
	float spec = pow(max(dot(normalEye, halfVector), 0.0f), material.shininess);
	float distance = length(spotLightPosition - fPosition);
	float attenuation = 1.0f / (spotConstant + spotLinear * distance + spotQuadratic * distance * distance);

	float theta = dot(lightDir, normalize(-spotLightDirection));
	float epsilon = spotlight1 - spotlight2;
	float intensity = clamp((theta - spotlight2)/epsilon, 0.0, 1.0);

	vec3 ambient = spotLightColor * spotLightAmbient * vec3(texture(diffuseTexture, fTexCoords));
	vec3 diffuse = spotLightColor * spotLightSpecular * diff * vec3(texture(diffuseTexture, fTexCoords));
	vec3 specular = spotLightColor * spotLightSpecular * spec * vec3(texture(specularTexture, fTexCoords));
	ambient *= attenuation * intensity;
	diffuse *= attenuation * intensity;
	specular *= attenuation * intensity;
	
    return ambient + diffuse + specular;
}


float computeFog()
{
    vec4 fPosEye = view * model * vec4(fPosition, 1.0f);

    float fogDensity = 0.009f;
    float fragmentDistance = length(fPosEye);
    float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));

    return clamp(fogFactor, 0.0f, 1.0f);
}


void main() 
{
    computeDirLight();

    //compute final vertex color
    vec3 color = min((ambient + diffuse) * texture(diffuseTexture, fTexCoords).rgb + specular * texture(specularTexture, fTexCoords).rgb, 1.0f);
    vec3 pointColor = computePointLight(pointLight, diffuseTexture, specularTexture);
    vec3 spotLightColor = computeLightSpotComponents();

    activatePointLight == 1 ? color += pointColor : color += vec3(0.0f);
    activateSpotLight == 1 ? color += spotLightColor : color += vec3(0.0f);

    if(activateFog == 1){
        float fogFactor = computeFog();
        vec4 fogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);
        fColor = mix(fogColor, vec4(color, 1.0f), fogFactor);
    }
    else {
        //output final color
        fColor = vec4(color, 1.0f);
    }


}
