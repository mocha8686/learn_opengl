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
		void use() const { glUseProgram(id); };
		GLuint getId() const { return id; };
};