#version 330 core

in vec3 normal;
in vec3 fragPos;
in vec3 lightPos;
out vec4 fragColor;

uniform vec3 objectColor;
uniform vec3 lightColor;

const float AMBIENT_LIGHT_STRENGTH = 0.1;
const float SPECULAR_LIGHT_STRENGTH = 0.5;
const int SHININESS = 32;

void main() {
	vec3 normal = normalize(normal);
	vec3 lightDir = normalize(lightPos - fragPos);
	float diffuseValue = max(dot(normal, lightDir), 0.0);

	vec3 viewDir = normalize(-fragPos);
	vec3 reflectDir = reflect(-lightDir, normal);
	float specularValue = pow(max(dot(viewDir, reflectDir), 0.0), 32);

	vec3 ambient = AMBIENT_LIGHT_STRENGTH * lightColor;
	vec3 diffuse = diffuseValue * lightColor;
	vec3 specular = SPECULAR_LIGHT_STRENGTH * specularValue * lightColor;

	fragColor = vec4(objectColor * (ambient + diffuse + specular), 1.0);
}