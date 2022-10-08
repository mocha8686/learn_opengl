#include "shader.hpp"

#include <glad/glad.h>

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

#define INFO_LOG_LENGTH 512

const char *shaderEnumToString(GLenum type) {
	switch (type) {
		case GL_VERTEX_SHADER:
			return "vertex shader";
		case GL_FRAGMENT_SHADER:
			return "fragment shader";
		default:
			return "shader";
	}
}

std::string read_file(const std::string &path) {
	std::ostringstream buf;
	std::ifstream file;

	file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	file.open(path);
	buf << file.rdbuf();
	file.close();

	return buf.str();
}

class Shader {
	private:
		GLuint id;
		GLenum type;

	public:
		Shader(GLenum type, const std::string &sourcePath) : type(type) {
			auto source = read_file(sourcePath);
			auto sourceStr = source.c_str();

			id = glCreateShader(type);
			glShaderSource(id, 1, &sourceStr, nullptr);
			glCompileShader(id);

			GLint success;
			glGetShaderiv(id, GL_COMPILE_STATUS, &success);
			if (!success) {
				GLchar infoLog[INFO_LOG_LENGTH];
				glGetShaderInfoLog(id, INFO_LOG_LENGTH, nullptr, infoLog);

				std::ostringstream what;
				const char *typeString = shaderEnumToString(type);
				what << "Error compiling " << typeString << " `" << sourcePath << "`: " << infoLog;
				throw std::runtime_error(what.str());
			}
		}
		~Shader() { glDeleteShader(id); };

		void attach(GLuint program) { glAttachShader(program, id); };
};

ShaderProgram::ShaderProgram(const std::string &vertexSourcePath, const std::string &fragmentSourcePath) {
	Shader vertexShader(GL_VERTEX_SHADER, vertexSourcePath);
	Shader fragmentShader(GL_FRAGMENT_SHADER, fragmentSourcePath);

	id = glCreateProgram();
	vertexShader.attach(id);
	fragmentShader.attach(id);
	glLinkProgram(id);

	GLint success;
	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if (!success) {
		char infoLog[INFO_LOG_LENGTH];
		glGetProgramInfoLog(id, INFO_LOG_LENGTH, nullptr, infoLog);
		std::ostringstream what;
		what << "Error linking shader program: " << infoLog;
		throw std::runtime_error(what.str());
	}
}