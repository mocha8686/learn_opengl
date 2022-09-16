#pragma once

#include <glad/glad.h>
#include <string>

bool initializeShaderProgram(GLuint &id);

class ShaderProgram {
	private:
		GLuint id;

	public:
		ShaderProgram(const std::string &vertexSourcePath, const std::string &fragmentSourcePath);
		~ShaderProgram() { glDeleteProgram(id); };
		inline void use() { glUseProgram(id); };
};