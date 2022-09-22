#include "texture.hpp"

#include <glad/glad.h>
#include <stb_image/stb_image.h>

#include <stdexcept>
#include <string>
#include <sstream>

Texture::Texture(const std::string &imagePath, GLint format) {
	stbi_set_flip_vertically_on_load(true);
	unsigned char *data = stbi_load(imagePath.c_str(), &width, &height, &channels, 0);
	if (!data) {
		std::ostringstream what;
		what << "Image `" << imagePath << "` failed to load.";
		throw std::runtime_error(what.str());
	}

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);
}

Texture::Texture(const std::string &imagePath, GLint wrapS, GLint wrapT, GLint minFilter, GLint magFilter, GLint format) {
	stbi_set_flip_vertically_on_load(true);
	unsigned char *data = stbi_load(imagePath.c_str(), &width, &height, &channels, 0);
	if (!data) {
		std::ostringstream what;
		what << "Image `" << imagePath << "` failed to load.";
		throw std::runtime_error(what.str());
	}

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);
}