#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <optional>

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

namespace Origo {

static glm::mat4 convert_matrix(const aiMatrix4x4& m) {
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

static void make_fallback_model(std::vector<Model::Node>& nodes, int& rootNode) {
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
    : m_path("")
    , m_model_shader_handle(std::nullopt) { }

Model::Model(const std::filesystem::path& path, const AssetHandle& shader)
    : m_path(path)
    , m_model_shader_handle(shader) { }

AssetType Model::get_asset_type() const { return AssetType::Model; }
AssetType Model::get_class_asset_type() { return AssetType::Model; }

const std::filesystem::path& Model::get_path() const { return m_path; }
int Model::get_root_node() const { return m_root_node; }
const std::vector<Model::Node>& Model::get_nodes() const { return m_nodes; }
const std::vector<Model::SubMesh>& Model::get_sub_meshes() const { return m_sub_meshes; }

void Model::set_path(const std::filesystem::path& path) { m_path = path; }
void Model::set_shader_handle(const AssetHandle& handle) { m_model_shader_handle = handle; }
void Model::set_shader_uuid(const UUID& id) { m_shader_uuid = id; }

void Model::load() {
	if (m_path.empty()) {
		ORG_CORE_WARN("Model::Load: path is empty");
		clear();
		make_fallback_model(m_nodes, m_root_node);
		return;
	}

	clear();
	load_from_assimp();

	if (m_nodes.empty() || m_root_node < 0 || m_root_node >= (int)m_nodes.size()) {
		ORG_CORE_WARN("Model::Load: invalid node graph produced for '{}', using fallback", m_path.string());
		make_fallback_model(m_nodes, m_root_node);
	}
}

void Model::clear() {
	m_sub_meshes.clear();
	m_nodes.clear();
	m_assimp_mesh_to_sub_mesh.clear();
	m_root_node = -1;
}

void Model::load_from_assimp() {
	constexpr unsigned int flags = aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_ImproveCacheLocality | aiProcess_LimitBoneWeights | aiProcess_ValidateDataStructure;

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(m_path.string(), flags);

	if (!scene || !scene->mRootNode) {
		ORG_CORE_ERROR("Assimp failed to load '{}': {}", m_path.string(), importer.GetErrorString());
		make_fallback_model(m_nodes, m_root_node);
		return;
	}

	m_assimp_mesh_to_sub_mesh.assign(scene->mNumMeshes, -1);
	m_sub_meshes.reserve(scene->mNumMeshes);

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

		int layout_id = VertexLayout::get_static_mesh_layout();
		int heap_id = GeometryHeapRegistry::get_or_create_static_mesh_heap(layout_id);
		GeometryHeap* heap = GeometryHeapRegistry::get_heap(heap_id);

		auto range = heap->allocate(
		    vertices.data(),
		    vertices.size() * sizeof(float),
		    8 * sizeof(float),
		    indices.data(),
		    indices.size());

		AssetHandle mesh_handle = AssetFactory::get_instance().create_runtime_asset<Mesh>(
		    "ModelMesh_" + std::to_string(i),
		    layout_id,
		    heap_id,
		    range);

		OptionalAssetHandle texture_handle {};
		aiMaterial* ai_mat = scene->mMaterials[mesh->mMaterialIndex];

		aiString tex_path;
		if (ai_mat && ai_mat->GetTexture(aiTextureType_DIFFUSE, 0, &tex_path) == AI_SUCCESS) {
			std::string path = tex_path.C_Str();

			texture_handle = AssetFactory::get_instance().create_runtime_asset<Texture2D>(
			    "ModelTex_" + std::to_string(i),
			    TextureType::Albedo);

			auto tex = AssetManager::get_instance().get_asset<Texture2D>(*texture_handle);

			if (!path.empty() && path[0] == '*') {
				int idx = std::atoi(path.c_str() + 1);
				if (idx >= 0 && idx < (int)scene->mNumTextures) {
					aiTexture* embedded = scene->mTextures[idx];
					if (embedded) {
						if (embedded->mHeight == 0) {
							std::vector<unsigned char> compressed(
							    reinterpret_cast<unsigned char*>(embedded->pcData),
							    reinterpret_cast<unsigned char*>(embedded->pcData) + embedded->mWidth);
							tex->set_source(MakeScope<TextureSourceEmbedded>(std::move(compressed)));
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

							tex->set_source(MakeScope<TextureSourceEmbedded>(w, h, 4, std::move(pixels)));
						}
					}
				}
			} else {
				tex->set_source(MakeScope<TextureSourceFile>((m_path.parent_path() / path).string()));
			}

			tex->load();
		}

		AssetHandle material_handle = AssetFactory::get_instance().create_runtime_asset<Material2D>(
		    "ModelMat_" + std::to_string(i),
		    *m_model_shader_handle,
		    texture_handle);

		auto material { AssetManager::get_instance().get_asset<Material2D>(material_handle) };
		material->set_textured();

		m_assimp_mesh_to_sub_mesh[i] = (int)m_sub_meshes.size();
		m_sub_meshes.push_back({ mesh_handle, material_handle });
	}

	m_root_node = process_node(scene->mRootNode, -1);
	if (m_nodes.empty() || m_root_node < 0 || m_root_node >= (int)m_nodes.size()) {
		make_fallback_model(m_nodes, m_root_node);
	}
}

int Model::process_node(aiNode* node, int parent) {
	const int index = (int)m_nodes.size();

	Node& dst = m_nodes.emplace_back();
	dst.Parent = parent;
	dst.LocalTransform = convert_matrix(node->mTransformation);

	dst.SubMeshIndex = -1;
	for (uint32_t k = 0; k < node->mNumMeshes; ++k) {
		const int assimp_mesh_idx = (int)node->mMeshes[k];
		if (assimp_mesh_idx >= 0 && assimp_mesh_idx < (int)m_assimp_mesh_to_sub_mesh.size()) {
			const int mapped = m_assimp_mesh_to_sub_mesh[assimp_mesh_idx];
			if (mapped != -1) {
				dst.SubMeshIndex = mapped;
				break;
			}
		}
	}

	dst.Children.reserve(node->mNumChildren);
	for (uint32_t i = 0; i < node->mNumChildren; ++i) {
		dst.Children.push_back(process_node(node->mChildren[i], index));
	}

	return index;
}

void Model::resolve() {
	if (m_path.empty()) {
		ORG_CORE_WARN("Could not Resolve Model because no path provided");
		return;
	}

	ensure_shader();

	load();
}

void Model::ensure_shader() {
	if (m_model_shader_handle.has_value())
		return;

	auto default_shader { DefaultAssetCache::get_instance().get_shader() };

	if (!m_shader_uuid.has_value()) {
		m_model_shader_handle = default_shader;
		return;
	}

	auto shader_handle = AssetManager::get_instance().get_handle_by_uuid(*m_shader_uuid);

	m_model_shader_handle = !shader_handle.has_value() ? default_shader : shader_handle;
}

OptionalAssetHandle Model::get_shader_handle() const { return m_model_shader_handle; }
}
