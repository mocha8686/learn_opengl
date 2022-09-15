#include <iostream>
#include <glad/glad.h>

const char *VERTEX_SHADER_SOURCE = "#version 330 core \
	layout (location = 0) in vec3 aPos; \
	layout (location = 1) in vec4 aCol; \
	out vec4 color; \
	void main() { \
		gl_Position = vec4(aPos, 1.0); \
		color = aCol; \
	}";
const char *FRAGMENT_SHADER_SOURCE = "#version 330 core \
	in vec4 color; \
	out vec4 FragColor; \
	void main() { \
		FragColor = color; \
	}";

bool initializeShader(GLuint type, GLsizei count, const GLchar *const *source, unsigned int &id) {
	id = glCreateShader(type);
	glShaderSource(id, count, source, nullptr);
	glCompileShader(id);

	int success;
	glGetShaderiv(id, GL_COMPILE_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetShaderInfoLog(id, 512, nullptr, infoLog);
		std::cerr << "Shader compilation failed:\n" << infoLog << std::endl;
		return false;
	}

	return true;
}

bool initializeShaderProgram(GLuint &id) {
	// Initialize vertex shader
	GLuint vertexShader;
	if (!initializeShader(GL_VERTEX_SHADER, 1, &VERTEX_SHADER_SOURCE, vertexShader)) {
		return false;
	}

	// Initialize fragment shader
	GLuint fragmentShader;
	if (!initializeShader(GL_FRAGMENT_SHADER, 1, &FRAGMENT_SHADER_SOURCE, fragmentShader)) {
		return false;
	}

	// Create shader program
	id = glCreateProgram();
	glAttachShader(id, vertexShader);
	glAttachShader(id, fragmentShader);
	glLinkProgram(id);

	// Check link status of shader program
	GLint success;
	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetProgramInfoLog(id, 512, nullptr, infoLog);
		std::cout << "Shader program linking failed:\n" << infoLog << std::endl;
		return false;
	}

	// Free shaders
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	return true;
}
