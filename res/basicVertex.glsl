#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aCol;
layout (location = 2) in vec2 aTexCoord;

out vec3 color;
out vec2 texCoord;

uniform mat4 trans;

void main() {
	color = aCol;
	texCoord = aTexCoord;
	gl_Position = trans * vec4(aPos, 1.0);
}