#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;


struct Material
{
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
};
uniform Material material;

struct DirLight
{
	vec3 direction;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform DirLight dirLight;

struct PointLight
{
	vec3 position;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
#define NR_POINT_LIGHTS 2
uniform PointLight pointLights[NR_POINT_LIGHTS];

vec3 CalculateDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction);

	//diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);

	//specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0f);

	//combine results
	vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, TexCoords));
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, TexCoords));
	vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, TexCoords));
	return (ambient + diffuse + specular);
}

vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);

	//diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);

	//specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0f);

	//attenuation
	float dist = length(light.position - fragPos);
	float attenuation = 1.0 / 
	(light.constant + light.linear * dist + light.quadratic * (dist*dist));

	//combine results
	vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, TexCoords));
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, TexCoords));
	vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, TexCoords));
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse + specular);
}

void main()
{    
	vec3 normal = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);
    
	vec3 result = CalculateDirLight(dirLight, normal, viewDir);

	for (int i = 0; i < NR_POINT_LIGHTS; i++)
		result += CalculatePointLight(pointLights[i], normal, FragPos, viewDir);

	FragColor = vec4(result, 1.0);
}