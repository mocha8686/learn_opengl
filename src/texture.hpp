#pragma once

#include <glad/glad.h>
#include <iosfwd>

enum TextureType {
	DIFFUSE,
	SPECULAR,
};

class Texture {
	private:
		GLuint id;
		TextureType type;
		int width, height, channels;

	public:
		Texture(
			const std::string &imagePath,
			TextureType type,
			GLint wrapS = GL_REPEAT,
			GLint wrapT = GL_REPEAT,
			GLint minFilter = GL_NEAREST_MIPMAP_NEAREST,
			GLint magFilter = GL_LINEAR
		);

		TextureType getType() const { return type; };

		void use(GLenum number) const {
			glActiveTexture(number);
			glBindTexture(GL_TEXTURE_2D, id);
		};
};