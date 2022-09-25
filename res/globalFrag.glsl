#version 330 core

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct Light {
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

in vec3 normal;
in vec3 fragPos;
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
	float diffuseValue = max(dot(normal, lightDir), 0.0);

	vec3 viewDir = normalize(-fragPos);
	vec3 reflectDir = reflect(-lightDir, normal);
	float specularValue = pow(max(dot(viewDir, reflectDir), 0.0), 32);

	vec3 ambient = light.ambient * material.ambient * lightColor;
	vec3 diffuse = light.diffuse * material.diffuse * diffuseValue * lightColor;
	vec3 specular = light.specular * material.specular * specularValue * lightColor;

	fragColor = vec4(ambient + diffuse + specular, 1.0);
}