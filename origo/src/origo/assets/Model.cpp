#include "origo/assets/MeshData.h"
#include "origo/assets/Model.h"
#include "origo/assets/AssetManager.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include <string>

namespace Origo {

#pragma region IO_ASSIMP
std::vector<MeshData> LoadMeshData(const std::string& path) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile("./resources/models/" + path,
	    aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs);

	std::vector<MeshData> meshes;

	if (!scene || !scene->HasMeshes()) {
		return meshes;
	}

	for (unsigned i = 0; i < scene->mNumMeshes; i++) {
		aiMesh* aMesh = scene->mMeshes[i];

		std::vector<float> vertices;
		std::vector<unsigned> indices;

		for (unsigned v = 0; v < aMesh->mNumVertices; v++) {
			// position
			vertices.push_back(aMesh->mVertices[v].x);
			vertices.push_back(aMesh->mVertices[v].y);
			vertices.push_back(aMesh->mVertices[v].z);

			// normal
			if (aMesh->HasNormals()) {
				vertices.push_back(aMesh->mNormals[v].x);
				vertices.push_back(aMesh->mNormals[v].y);
				vertices.push_back(aMesh->mNormals[v].z);
			} else {
				vertices.insert(vertices.end(), { 0, 0, 0 });
			}

			// texcoords
			if (aMesh->HasTextureCoords(0)) {
				vertices.push_back(aMesh->mTextureCoords[0][v].x);
				vertices.push_back(aMesh->mTextureCoords[0][v].y);
			} else {
				vertices.insert(vertices.end(), { 0, 0 });
			}
		}

		for (unsigned f = 0; f < aMesh->mNumFaces; f++) {
			const aiFace& face = aMesh->mFaces[f];
			for (unsigned j = 0; j < face.mNumIndices; j++) {
				indices.push_back(face.mIndices[j]);
			}
		}

		meshes.emplace_back(vertices, indices);
	}

	return meshes;
}
#pragma endregion

Model::Model(std::string_view path)
    : m_Path(path)
    , m_Meshes() {
	auto meshData { LoadMeshData(m_Path) };

	for (size_t i {}; i < meshData.size(); i++) {
		auto mesh { AssetManager::CreateAsset<Mesh>(path.data() + std::to_string('_') + std::to_string(i), m_Path, i, meshData[i]) };
		m_Meshes.push_back(mesh);
	}
}

void Model::Render() const {
	for (const auto& mesh : m_Meshes) {
		mesh->Render();
	}
}

}
