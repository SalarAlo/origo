#include "origo/assets/MeshData.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

namespace Origo {

#pragma region IO_ASSIMP
std::vector<MeshData> Create(const std::string& path) {
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

}
