#include "model.hpp"
#include "../context.hpp"
#include "mesh.hpp"
#include "texture.hpp"

#include <assimp/Importer.hpp>
#include <assimp/material.h>
#include <assimp/material.inl>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/types.h>
#include <assimp/vector3.h>

#include <memory>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>

void Model::draw(ShaderProgram &shader) {
	for (unsigned int i = 0; i < meshes.size(); i++) {
		meshes[i].draw(shader);
	}
}

void Model::loadModel(const std::string &path) {
	Assimp::Importer importer;
	const auto *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::ostringstream what;
		what << "Failed to load model `" << path << "`: " << importer.GetErrorString();
		throw std::runtime_error(what.str());
	}

	dir = path.substr(0, path.find_last_of('/'));
	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene) {
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		auto *mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<std::shared_ptr<Texture>> textures;

	// Vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		auto v = mesh->mVertices[i];
		Vertex vertex {
			.position = glm::vec3(v.x, v.y, v.z)
		};

		if (mesh->HasNormals()) {
			auto n = mesh->mNormals[i];
			vertex.normal = glm::vec3(n.x, n.y, n.x);
		}

		if (mesh->mTextureCoords[0]) {
			auto tc = mesh->mTextureCoords[0][i];
			vertex.texCoords = glm::vec2(tc.x, tc.y);
		} else {
			vertex.texCoords = glm::vec2(0.0f);
		}

		vertices.push_back(vertex);
	}

	// Indices
	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		auto face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}

	// Materials
	auto mIdx = mesh->mMaterialIndex;
	if (mIdx >= 0) {
		auto *material = scene->mMaterials[mIdx];

		auto diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		auto specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}

	return Mesh(ctx, vertices, indices, textures);
}

std::vector<std::shared_ptr<Texture>> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type) {
	std::vector<std::shared_ptr<Texture>> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
		aiString str;
		mat->GetTexture(type, i, &str);
		auto filename = std::string(str.C_Str());
		auto textureOpt = ctx.textures.get(filename);

		if (!textureOpt) {
			TextureType enumType;
			switch (type) {
				case aiTextureType_DIFFUSE:
					enumType = DIFFUSE;
					break;
				case aiTextureType_SPECULAR:
					enumType = SPECULAR;
					break;
				default:
					throw std::invalid_argument("Texture type is not yet supported.");
			}

			auto texture = std::make_shared<Texture>(dir + "/" + filename, enumType);
			ctx.textures.set(filename, texture);
			textureOpt = ctx.textures.get(filename);
		}

		textures.push_back(textureOpt.value());
	}

	return textures;
}