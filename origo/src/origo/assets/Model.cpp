#include "origo/assets/Material.h"
#include "origo/assets/Mesh.h"
#include "origo/assets/MeshData.h"
#include "origo/assets/Model.h"
#include "origo/assets/AssetManager.h"
#include "origo/assets/Texture.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "origo/core/Logger.h"

#include <filesystem>
#include <stdexcept>
#include <string>

namespace Origo {

static MeshData ConvertMesh(const aiMesh* aMesh) {
	std::vector<float> vertices;
	std::vector<unsigned> indices;
	vertices.reserve(aMesh->mNumVertices * 8); // (pos) 3 + (normal) 3 + (uv) 2. 3+3+2=8 thats why *8.

	for (unsigned v = 0; v < aMesh->mNumVertices; ++v) {
		vertices.push_back(aMesh->mVertices[v].x);
		vertices.push_back(aMesh->mVertices[v].y);
		vertices.push_back(aMesh->mVertices[v].z);

		if (aMesh->HasNormals()) {
			vertices.push_back(aMesh->mNormals[v].x);
			vertices.push_back(aMesh->mNormals[v].y);
			vertices.push_back(aMesh->mNormals[v].z);
		} else {
			vertices.insert(vertices.end(), { 0.0f, 0.0f, 0.0f });
		}

		if (aMesh->HasTextureCoords(0)) {
			vertices.push_back(aMesh->mTextureCoords[0][v].x);
			vertices.push_back(aMesh->mTextureCoords[0][v].y);
		} else {
			vertices.insert(vertices.end(), { 0.0f, 0.0f });
		}
	}

	for (unsigned f = 0; f < aMesh->mNumFaces; ++f) {
		const aiFace& face = aMesh->mFaces[f];
		for (unsigned j = 0; j < face.mNumIndices; ++j)
			indices.push_back(face.mIndices[j]);
	}

	return MeshData(vertices, indices);
}

static RID ExtractTexture(
    const aiScene* scene,
    const aiMaterial* material,
    const std::string& modelPath,
    unsigned meshIndex) {
	aiString texPath;
	if (material->GetTexture(aiTextureType_BASE_COLOR, 0, &texPath) != AI_SUCCESS)
		material->GetTexture(aiTextureType_DIFFUSE, 0, &texPath);

	if (texPath.length == 0)
		throw std::runtime_error("Cant create Texture with the given data");

	if (texPath.C_Str()[0] == '*') {
		const unsigned index = std::atoi(texPath.C_Str() + 1);
		const aiTexture* embeddedTex = scene->mTextures[index];
		const std::string texName = std::filesystem::path(modelPath).stem().string() + "_embedded_" + std::to_string(index);

		if (embeddedTex->mHeight == 0) {
			return AssetManager::CreateAsset<Texture>(
			    texName,
			    embeddedTex,
			    TextureType::Albedo);
		}
	}

	std::string relPath = texPath.C_Str();

	std::filesystem::path modelDir = std::filesystem::path(modelPath).parent_path();
	std::filesystem::path fullPath = modelDir / relPath;
	fullPath = std::filesystem::weakly_canonical(fullPath);

	std::string texName = fullPath.filename().string();

	ORG_INFO("Loading texture: {}", fullPath.string());

	return AssetManager::CreateAsset<Texture>(
	    texName,
	    fullPath.string(),
	    TextureType::Albedo);
}

Model::Model(std::string_view path, RID shader)
    : m_Path(path) {
	const std::string assetRoot = "resources/models/";
	const std::string absolutePath = assetRoot + std::string(path);

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(
	    absolutePath,
	    aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);

	if (!scene || !scene->HasMeshes()) {
		ORG_ERROR("[Model] Failed to load model at {}", absolutePath);
		return;
	}

	for (unsigned i = 0; i < scene->mNumMeshes; ++i) {
		aiMesh* aMesh = scene->mMeshes[i];
		aiMaterial* material = scene->mMaterials[aMesh->mMaterialIndex];

		std::string baseName = std::filesystem::path(m_Path).stem().string() + "_mesh_" + std::to_string(i);

		MeshData meshData = ConvertMesh(aMesh);
		try {
			RID texture = ExtractTexture(scene, material, absolutePath, i);

			RID mesh = AssetManager::CreateAsset<Mesh>(baseName, m_Path, i, meshData);
			RID mat = AssetManager::CreateAsset<Material>(baseName + "_mat", shader, texture, false);
			m_SubMeshes.push_back({ mesh, mat });
		} catch (std::runtime_error e) {
			ORG_CORE_ERROR("{}", e.what());
		}
	}
}

void Model::Render() const {
	for (const auto& sub : m_SubMeshes) {
		auto material { AssetManager::GetAssetAs<Material>(sub.material) };
		auto mesh { AssetManager::GetAssetAs<Mesh>(sub.mesh) };
		material->Bind();
		mesh->Render();
	}
}

}
