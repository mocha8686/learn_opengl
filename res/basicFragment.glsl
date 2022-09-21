#version 330 core

in vec3 color;
in vec2 texCoord;
out vec4 FragColor;

uniform float mixPercent;
uniform sampler2D tex0;
uniform sampler2D tex1;

void main() {
	FragColor = mix(texture(tex0, texCoord), texture(tex1, texCoord), mixPercent);
}