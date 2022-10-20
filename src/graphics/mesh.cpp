#include "mesh.hpp"

#include "shader.hpp"
#include "texture.hpp"
#include <glad/glad.h>
#include <iosfwd>
#include <stddef.h>
#include <string>

const std::string textureTypeToString(TextureType type) {
	switch (type) {
		case DIFFUSE:
			return "Diffuse";
		case SPECULAR:
			return "Specular";
	}
}

void Mesh::setupMesh() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

	GLsizei stride = sizeof(Vertex);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*) 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*) offsetof(Vertex, normal));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid*) offsetof(Vertex, texCoords));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
}

void Mesh::draw(ShaderProgram &shader) {
	unsigned int diffuseN = 0;
	unsigned int specularN = 0;
	for (unsigned int i = 0; i < textures.size(); i++) {
		auto texture = textures[i];
		auto type = textures[i]->getType();
		texture->use(GL_TEXTURE0 + i);

		std::string number;
		switch (type) {
			case DIFFUSE:
				number = std::to_string(diffuseN++);
				break;
			case SPECULAR:
				number = std::to_string(specularN++);
				break;
		}

		auto str = "material.tex" + textureTypeToString(type) + number;
		shader.tryUniformInt(str.c_str(), i);
		texture->use(GL_TEXTURE0 + i);
	}

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(0);
}