#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
out vec3 normal;
out vec3 fragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;

void main() {
	normal = normalMatrix * aNormal;
	fragPos = (model * vec4(aPos, 1.0)).xyz;
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}