#pragma once

#include <glad/glad.h>

#include <string>

class Texture {
	private:
		GLuint id;
		int width, height, channels;

	public:
		Texture(const std::string &imagePath);
		Texture(const std::string &imagePath, GLint wrapS, GLint wrapT, GLint minFilter, GLint magFilter, GLint format);

		void use(GLenum number) const {
			glActiveTexture(number);
			glBindTexture(GL_TEXTURE_2D, id);
		};
};