#pragma once

#include "shader.hpp"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
};

enum TextureType {
	DIFFUSE,
	SPECULAR,
};

struct Texture {
	GLuint id;
	TextureType type;
};

class Mesh {
	private:
		GLuint VAO, VBO, EBO;
		void setupMesh();

	public:
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture> textures;

		Mesh(
			std::vector<Vertex> vertices,
			std::vector<unsigned int> indices,
			std::vector<Texture> textures
		) : vertices(vertices), indices(indices), textures(textures) {
			setupMesh();
		};
		~Mesh();

		void draw(ShaderProgram &shader);
};