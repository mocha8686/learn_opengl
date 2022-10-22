#pragma once

#include "mesh.hpp"
#include <assimp/material.h>
#include <iosfwd>
#include <memory>
#include <vector>

class Context;
class ShaderProgram;
class Texture;
struct aiMesh;
struct aiNode;
struct aiScene;

class Model {
	private:
		Context &ctx;
		std::vector<Mesh> meshes;
		std::string dir;

		void loadModel(const std::string &path);
		void processNode(aiNode *node, const aiScene *scene);
		Mesh processMesh(aiMesh *mesh, const aiScene *scene);
		std::vector<std::shared_ptr<Texture>> loadMaterialTextures(aiMaterial *mat, aiTextureType type);

	public:
		Model(Context &ctx, const std::string &path) : ctx(ctx) { loadModel(path); };
		void draw(std::shared_ptr<ShaderProgram> shader);
};