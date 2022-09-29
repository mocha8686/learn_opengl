#version 330 core

struct Material {
	sampler2D diffuseMap;
	sampler2D specularMap;
	float shininess;
};

struct Light {
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float linear;
	float quadratic;
};

in vec3 normal;
in vec3 fragPos;
in vec2 texCoords;
out vec4 fragColor;

uniform Material material;
uniform Light light;
uniform vec3 lightColor;

const float AMBIENT_LIGHT_STRENGTH = 0.1;
const float SPECULAR_LIGHT_STRENGTH = 0.5;
const int SHININESS = 32;

void main() {
	vec3 normal = normalize(normal);
	vec3 lightDir = normalize(light.position - fragPos);

	vec3 diffuseMapValue = texture(material.diffuseMap, texCoords).rgb;
	float diffuseValue = max(dot(normal, lightDir), 0.0);

	vec3 viewDir = normalize(-fragPos);
	vec3 reflectDir = reflect(-lightDir, normal);
	float specularValue = pow(max(dot(viewDir, reflectDir), 0.0), 32);

	vec3 ambient = light.ambient * diffuseMapValue;
	vec3 diffuse = light.diffuse * diffuseMapValue * diffuseValue;
	vec3 specular = light.specular * texture(material.specularMap, texCoords).rgb * specularValue;

	float dist = distance(light.position, fragPos);
	float attenuation = 1.0 / (1.0 + light.linear * dist + light.quadratic * dist * dist);

	fragColor = vec4((ambient + diffuse + specular) * lightColor * attenuation, 1.0);
}