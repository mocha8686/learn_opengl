#version 330 core

in vec3 normal;
in vec3 fragPos;
out vec4 fragColor;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;

const float AMBIENT_LIGHT_STRENGTH = 0.1;

void main() {
	vec3 normal = normalize(normal);
	vec3 lightDir = normalize(lightPos - fragPos);
	float diffuseStrength = max(dot(normal, lightDir), 0.0);

	vec3 ambient = AMBIENT_LIGHT_STRENGTH * lightColor;
	vec3 diffuse = diffuseStrength * lightColor;

	fragColor = vec4(objectColor * (ambient + diffuse), 1.0);
}