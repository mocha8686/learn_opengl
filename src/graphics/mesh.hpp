#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>
#include <vector>

class Context;
class ShaderProgram;
class Texture;

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
};

class Mesh {
	private:
		Context &ctx;
		GLuint VAO, VBO, EBO;
		void setupMesh();

	public:
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<std::shared_ptr<Texture>> textures;

		Mesh(
			Context &ctx,
			std::vector<Vertex> vertices,
			std::vector<unsigned int> indices,
			std::vector<std::shared_ptr<Texture>> textures
		) : ctx(ctx), vertices(vertices), indices(indices), textures(textures) {
			setupMesh();
		};

		void draw(std::shared_ptr<ShaderProgram> shader);
};