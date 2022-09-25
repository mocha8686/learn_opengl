#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
out vec3 normal;
out vec3 fragPos;
out vec3 lightPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;
uniform vec3 worldLightPos;

void main() {
	normal = normalMatrix * aNormal;
	fragPos = (view * model * vec4(aPos, 1.0)).xyz;
	lightPos = (view * model * vec4(worldLightPos, 1.0)).xyz;
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}