#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <sstream>
#include <stdexcept>

class ShaderProgram {
	private:
		GLuint id;
		GLint location(const std::string &name) const {
			GLint res = glGetUniformLocation(id, name.c_str());
			if (res == -1) {
				std::ostringstream what;
				what << "No uniform with name `" << name << "` in shader.";
				throw std::runtime_error(what.str());
			}
			return res;
		};
		GLint tryLocation(const std::string &name) const {
			return glGetUniformLocation(id, name.c_str());
		};

	public:
		ShaderProgram(const std::string &vertexSourcePath, const std::string &fragmentSourcePath);
		~ShaderProgram() { glDeleteProgram(id); };
		void use() const { glUseProgram(id); };

		void uniformBool(const std::string &name, bool value) const { use(); glUniform1i(location(name), value); };
		void tryUniformBool(const std::string &name, bool value) const { use(); glUniform1i(tryLocation(name), value); };
		void uniformInt(const std::string &name, int value) const { use(); glUniform1i(location(name), value); };
		void tryUniformInt(const std::string &name, int value) const { use(); glUniform1i(tryLocation(name), value); };
		void uniformFloat(const std::string &name, float value) const { use(); glUniform1f(location(name), value); };
		void tryUniformFloat(const std::string &name, float value) const { use(); glUniform1f(tryLocation(name), value); };

		void uniformVec2(const std::string &name, float x, float y) const { use(); glUniform2f(location(name), x, y); };
		void tryUniformVec2(const std::string &name, float x, float y) const { use(); glUniform2f(tryLocation(name), x, y); };
		void uniformVec2(const std::string &name, float value) const { use(); glUniform2f(location(name), value, value); };
		void tryUniformVec2(const std::string &name, float value) const { use(); glUniform2f(tryLocation(name), value, value); };
		void uniformVec2(const std::string &name, const glm::vec2 &value) const { use(); glUniform2fv(location(name), 1, glm::value_ptr(value)); };
		void tryUniformVec2(const std::string &name, const glm::vec2 &value) const { use(); glUniform2fv(tryLocation(name), 1, glm::value_ptr(value)); };

		void uniformVec3(const std::string &name, float x, float y, float z) const { use(); glUniform3f(location(name), x, y, z); };
		void tryUniformVec3(const std::string &name, float x, float y, float z) const { use(); glUniform3f(tryLocation(name), x, y, z); };
		void uniformVec3(const std::string &name, float value) const { use(); glUniform3f(location(name), value, value, value); };
		void tryUniformVec3(const std::string &name, float value) const { use(); glUniform3f(tryLocation(name), value, value, value); };
		void uniformVec3(const std::string &name, const glm::vec3 &value) const { use(); glUniform3fv(location(name), 1, glm::value_ptr(value)); };
		void tryUniformVec3(const std::string &name, const glm::vec3 &value) const { use(); glUniform3fv(tryLocation(name), 1, glm::value_ptr(value)); };

		void uniformVec4(const std::string &name, float x, float y, float z, float w) const { use(); glUniform4f(location(name), x, y, z, w); };
		void tryUniformVec4(const std::string &name, float x, float y, float z, float w) const { use(); glUniform4f(tryLocation(name), x, y, z, w); };
		void uniformVec4(const std::string &name, float value) const { use(); glUniform4f(location(name), value, value, value, value); };
		void tryUniformVec4(const std::string &name, float value) const { use(); glUniform4f(tryLocation(name), value, value, value, value); };
		void uniformVec4(const std::string &name, const glm::vec4 &value) const { use(); glUniform4fv(location(name), 1, glm::value_ptr(value)); };
		void tryUniformVec4(const std::string &name, const glm::vec4 &value) const { use(); glUniform4fv(tryLocation(name), 1, glm::value_ptr(value)); };

		void uniformMat2(const std::string &name, const glm::mat2 &value) const { use(); glUniformMatrix2fv(location(name), 1, GL_FALSE, glm::value_ptr(value)); };
		void tryUniformMat2(const std::string &name, const glm::mat2 &value) const { use(); glUniformMatrix2fv(tryLocation(name), 1, GL_FALSE, glm::value_ptr(value)); };
		void uniformMat3(const std::string &name, const glm::mat3 &value) const { use(); glUniformMatrix3fv(location(name), 1, GL_FALSE, glm::value_ptr(value)); };
		void tryUniformMat3(const std::string &name, const glm::mat3 &value) const { use(); glUniformMatrix3fv(tryLocation(name), 1, GL_FALSE, glm::value_ptr(value)); };
		void uniformMat4(const std::string &name, const glm::mat4 &value) const { use(); glUniformMatrix4fv(location(name), 1, GL_FALSE, glm::value_ptr(value)); };
		void tryUniformMat4(const std::string &name, const glm::mat4 &value) const { use(); glUniformMatrix4fv(tryLocation(name), 1, GL_FALSE, glm::value_ptr(value)); };
};