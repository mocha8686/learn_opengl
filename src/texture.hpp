#pragma once

#include <glad/glad.h>

#include <string>

class Texture {
	private:
		GLuint id;
		int width, height, channels;

	public:
		Texture(const std::string &imagePath, GLint format);
		Texture(const std::string &imagePath, GLint wrapS, GLint wrapT, GLint minFilter, GLint magFilter, GLint format);

		void use(GLenum texture) {
			glActiveTexture(texture);
			glBindTexture(GL_TEXTURE_2D, id);
		};
};