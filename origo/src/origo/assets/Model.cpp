#include "Model.h"

#include "origo/assets/AssetFactory.h"
#include "origo/assets/AssetManager.h"
#include "origo/assets/DefaultAssetCache.h"
#include "origo/assets/Material2D.h"
#include "origo/assets/Mesh.h"
#include "origo/assets/Texture2D.h"
#include "origo/assets/TextureSource.h"
#include "origo/renderer/GeometryHeapRegistry.h"
#include "origo/renderer/VertexLayout.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <optional>

namespace Origo {

static glm::mat4 ConvertMatrix(const aiMatrix4x4& m) {
	glm::mat4 out;
	out[0][0] = m.a1;
	out[1][0] = m.a2;
	out[2][0] = m.a3;
	out[3][0] = m.a4;
	out[0][1] = m.b1;
	out[1][1] = m.b2;
	out[2][1] = m.b3;
	out[3][1] = m.b4;
	out[0][2] = m.c1;
	out[1][2] = m.c2;
	out[2][2] = m.c3;
	out[3][2] = m.c4;
	out[0][3] = m.d1;
	out[1][3] = m.d2;
	out[2][3] = m.d3;
	out[3][3] = m.d4;
	return out;
}

static void MakeFallbackModel(std::vector<Model::Node>& nodes, int& rootNode) {
	nodes.clear();
	nodes.reserve(1);

	Model::Node& n = nodes.emplace_back();
	n.Parent = -1;
	n.SubMeshIndex = -1;
	n.LocalTransform = glm::mat4(1.0f);
	n.Children.clear();

	rootNode = 0;
}

Model::Model()
    : m_Path("")
    , m_ModelShaderHandle(std::nullopt) { }

Model::Model(const std::filesystem::path& path, const AssetHandle& shader)
    : m_Path(path)
    , m_ModelShaderHandle(shader) { }

AssetType Model::GetAssetType() const { return AssetType::Model; }
AssetType Model::GetClassAssetType() { return AssetType::Model; }

const std::filesystem::path& Model::GetPath() const { return m_Path; }
int Model::GetRootNode() const { return m_RootNode; }
const std::vector<Model::Node>& Model::GetNodes() const { return m_Nodes; }
const std::vector<Model::SubMesh>& Model::GetSubMeshes() const { return m_SubMeshes; }

void Model::SetPath(const std::filesystem::path& path) { m_Path = path; }
void Model::SetShaderHandle(const AssetHandle& handle) { m_ModelShaderHandle = handle; }
void Model::SetShaderUUID(const UUID& id) { m_ShaderUUID = id; }

void Model::Load() {
	if (m_Path.empty()) {
		ORG_CORE_WARN("Model::Load: path is empty");
		Clear();
		MakeFallbackModel(m_Nodes, m_RootNode);
		return;
	}

	Clear();
	LoadFromAssimp();

	if (m_Nodes.empty() || m_RootNode < 0 || m_RootNode >= (int)m_Nodes.size()) {
		ORG_CORE_WARN("Model::Load: invalid node graph produced for '{}', using fallback", m_Path.string());
		MakeFallbackModel(m_Nodes, m_RootNode);
	}
}

void Model::Clear() {
	m_SubMeshes.clear();
	m_Nodes.clear();
	m_AssimpMeshToSubMesh.clear();
	m_RootNode = -1;
}

void Model::LoadFromAssimp() {
	constexpr unsigned int Flags = aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_ImproveCacheLocality | aiProcess_LimitBoneWeights | aiProcess_ValidateDataStructure;

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(m_Path.string(), Flags);

	if (!scene || !scene->mRootNode) {
		ORG_CORE_ERROR("Assimp failed to load '{}': {}", m_Path.string(), importer.GetErrorString());
		MakeFallbackModel(m_Nodes, m_RootNode);
		return;
	}

	m_AssimpMeshToSubMesh.assign(scene->mNumMeshes, -1);
	m_SubMeshes.reserve(scene->mNumMeshes);

	for (uint32_t i = 0; i < scene->mNumMeshes; ++i) {
		aiMesh* mesh = scene->mMeshes[i];
		if (!mesh || mesh->mNumVertices == 0)
			continue;

		if (mesh->mMaterialIndex >= scene->mNumMaterials)
			continue;

		std::vector<float> vertices;
		std::vector<uint32_t> indices;

		vertices.reserve(mesh->mNumVertices * 8);

		for (uint32_t v = 0; v < mesh->mNumVertices; ++v) {
			const auto& p = mesh->mVertices[v];
			const auto& n = mesh->mNormals[v];

			vertices.push_back(p.x);
			vertices.push_back(p.y);
			vertices.push_back(p.z);

			vertices.push_back(n.x);
			vertices.push_back(n.y);
			vertices.push_back(n.z);

			if (mesh->mTextureCoords[0]) {
				vertices.push_back(1.0f - mesh->mTextureCoords[0][v].x);
				vertices.push_back(1.0f - mesh->mTextureCoords[0][v].y);
			} else {
				vertices.push_back(0.0f);
				vertices.push_back(0.0f);
			}
		}

		for (uint32_t f = 0; f < mesh->mNumFaces; ++f) {
			const aiFace& face = mesh->mFaces[f];
			if (face.mNumIndices != 3)
				continue;
			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}

		int layoutId = VertexLayout::GetStaticMeshLayout();
		int heapId = GeometryHeapRegistry::GetOrCreateStaticMeshHeap(layoutId);
		GeometryHeap* heap = GeometryHeapRegistry::GetHeap(heapId);

		auto range = heap->Allocate(
		    vertices.data(),
		    vertices.size() * sizeof(float),
		    8 * sizeof(float),
		    indices.data(),
		    indices.size());

		AssetHandle meshHandle = AssetFactory::CreateRuntimeAsset<Mesh>(
		    "ModelMesh_" + std::to_string(i),
		    layoutId,
		    heapId,
		    range);

		OptionalAssetHandle textureHandle {};
		aiMaterial* aiMat = scene->mMaterials[mesh->mMaterialIndex];

		aiString texPath;
		if (aiMat && aiMat->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == AI_SUCCESS) {
			std::string path = texPath.C_Str();

			textureHandle = AssetFactory::CreateRuntimeAsset<Texture2D>(
			    "ModelTex_" + std::to_string(i),
			    TextureType::Albedo);

			auto tex = AssetManager::GetInstance().Get<Texture2D>(*textureHandle);

			if (!path.empty() && path[0] == '*') {
				int idx = std::atoi(path.c_str() + 1);
				if (idx >= 0 && idx < (int)scene->mNumTextures) {
					aiTexture* embedded = scene->mTextures[idx];
					if (embedded) {
						if (embedded->mHeight == 0) {
							std::vector<unsigned char> compressed(
							    reinterpret_cast<unsigned char*>(embedded->pcData),
							    reinterpret_cast<unsigned char*>(embedded->pcData) + embedded->mWidth);
							tex->SetSource(MakeScope<TextureSourceEmbedded>(std::move(compressed)));
						} else {
							const int w = embedded->mWidth;
							const int h = embedded->mHeight;

							std::vector<unsigned char> pixels((size_t)w * h * 4);
							const aiTexel* src = reinterpret_cast<const aiTexel*>(embedded->pcData);

							for (int p = 0; p < w * h; ++p) {
								pixels[p * 4 + 0] = src[p].r;
								pixels[p * 4 + 1] = src[p].g;
								pixels[p * 4 + 2] = src[p].b;
								pixels[p * 4 + 3] = src[p].a;
							}

							tex->SetSource(MakeScope<TextureSourceEmbedded>(w, h, 4, std::move(pixels)));
						}
					}
				}
			} else {
				tex->SetSource(MakeScope<TextureSourceFile>((m_Path.parent_path() / path).string()));
			}

			tex->Load();
		}

		AssetHandle materialHandle = AssetFactory::CreateRuntimeAsset<Material2D>(
		    "ModelMat_" + std::to_string(i),
		    *m_ModelShaderHandle,
		    textureHandle);

		m_AssimpMeshToSubMesh[i] = (int)m_SubMeshes.size();
		m_SubMeshes.push_back({ meshHandle, materialHandle });
	}

	m_RootNode = ProcessNode(scene->mRootNode, -1);
	if (m_Nodes.empty() || m_RootNode < 0 || m_RootNode >= (int)m_Nodes.size()) {
		MakeFallbackModel(m_Nodes, m_RootNode);
	}
}

int Model::ProcessNode(aiNode* node, int parent) {
	const int index = (int)m_Nodes.size();

	Node& dst = m_Nodes.emplace_back();
	dst.Parent = parent;
	dst.LocalTransform = ConvertMatrix(node->mTransformation);

	dst.SubMeshIndex = -1;
	for (uint32_t k = 0; k < node->mNumMeshes; ++k) {
		const int assimpMeshIdx = (int)node->mMeshes[k];
		if (assimpMeshIdx >= 0 && assimpMeshIdx < (int)m_AssimpMeshToSubMesh.size()) {
			const int mapped = m_AssimpMeshToSubMesh[assimpMeshIdx];
			if (mapped != -1) {
				dst.SubMeshIndex = mapped;
				break;
			}
		}
	}

	dst.Children.reserve(node->mNumChildren);
	for (uint32_t i = 0; i < node->mNumChildren; ++i) {
		dst.Children.push_back(ProcessNode(node->mChildren[i], index));
	}

	return index;
}

void Model::Resolve() {
	if (m_Path.empty()) {
		ORG_CORE_WARN("Could not Resolve Model because no path provided");
		return;
	}

	EnsureShader();

	Load();
}

void Model::EnsureShader() {
	if (m_ModelShaderHandle.has_value())
		return;

	auto defaultShader { DefaultAssetCache::GetInstance().GetShader() };

	if (!m_ShaderUUID.has_value()) {
		m_ModelShaderHandle = defaultShader;
		return;
	}

	auto shaderHandle = AssetManager::GetInstance().GetHandleByUUID(*m_ShaderUUID);

	m_ModelShaderHandle = !shaderHandle.has_value() ? defaultShader : shaderHandle;
}

OptionalAssetHandle Model::GetShaderHandle() const { return m_ModelShaderHandle; }
}
