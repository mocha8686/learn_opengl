#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 fFragPos;
out vec3 fNormal;
out vec2 fTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;

void main() {
	vec3 fragPos = (view * model * vec4(aPos, 1.0)).xyz;
	fFragPos = fragPos;
	fNormal = mat3(view) * normalMatrix * aNormal;
	fTexCoords = aTexCoords;

	gl_Position = projection * vec4(fragPos, 1.0);
}