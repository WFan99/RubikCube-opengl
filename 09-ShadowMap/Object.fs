#version 330 core

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

struct DirctionLight {
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight {
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

in vec3 fragPos;
in vec4 shadowPos;
in vec3 normal;
in vec2 texCoord;

out vec4 FragColor;

uniform vec3 viewPos;
uniform Material material;
uniform sampler2D shadowMap;
//uniform DirctionLight directionLight;
uniform PointLight pointLight;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
	float bias = 0.005;
    // check whether current frag pos is in shadow
    float shadow = currentDepth-0.005 > closestDepth  ? 1.0 : 0.0;

    return shadow;
}


void main()
{
	FragColor = texture(material.diffuse,texCoord);
	// ambient
	vec3 ambientColor =  pointLight.ambient * FragColor.rgb ;
	// diffuse
	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(pointLight.position - fragPos);
	float diff = max(dot(norm,lightDir),0.0);
	vec3 diffuseColor = diff * pointLight.diffuse * FragColor.rgb;
	// specular
	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 halfDir = normalize((lightDir+viewDir)/2);
	float spec = pow(max(dot(halfDir,norm),0.0), material.shininess);
	vec3 specularColor = spec * pointLight.specular * vec3(texture(material.specular, texCoord));

	float shadow = ShadowCalculation(shadowPos);

	FragColor = vec4(ambientColor + (1.0-shadow)*( diffuseColor + specularColor), 1.0);

}