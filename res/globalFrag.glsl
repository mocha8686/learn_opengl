#version 330 core

in vec3 normal;
in vec3 fragPos;
out vec4 fragColor;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

const float AMBIENT_LIGHT_STRENGTH = 0.1;
const float SPECULAR_LIGHT_STRENGTH = 0.5;
const int SHININESS = 32;

void main() {
	vec3 normal = normalize(normal);
	vec3 lightDir = normalize(lightPos - fragPos);
	float diffuseStrength = max(dot(normal, lightDir), 0.0);

	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, normal);
	float specularStrength = pow(max(dot(viewDir, reflectDir), 0.0), 32);

	vec3 ambient = AMBIENT_LIGHT_STRENGTH * lightColor;
	vec3 diffuse = diffuseStrength * lightColor;
	vec3 specular = specularStrength * lightColor;

	fragColor = vec4(objectColor * (ambient + diffuse + specular), 1.0);
}