#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <cmath>

#include <string>

#include "Model.h"
#include "ModelInternal.h"

#include "origo/assets/AssetFactory.h"
#include "origo/assets/AssetManager.h"
#include "origo/assets/DefaultAssetCache.h"
#include "origo/assets/Mesh.h"

#include "origo/assets/material/MaterialPBR.h"

#include "origo/renderer/GeometryHeapRegistry.h"
#include "origo/renderer/VertexLayout.h"

namespace Origo {

namespace {
	constexpr unsigned int k_assimp_load_flags = aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_ImproveCacheLocality | aiProcess_LimitBoneWeights | aiProcess_ValidateDataStructure;

	struct ImportedTextureHandles {
		OptionalAssetHandle Albedo;
		OptionalAssetHandle Normal;
		OptionalAssetHandle MetallicRoughness;
		OptionalAssetHandle Ao;
		OptionalAssetHandle Emissive;
	};

	struct Bounds {
		glm::vec3 Min { 0.0f };
		glm::vec3 Max { 0.0f };
	};

	void append_vertex_data(std::vector<float>& vertices, const aiMesh& mesh, uint32_t vertex_index) {
		const aiVector3D& position = mesh.mVertices[vertex_index];
		const aiVector3D& normal = mesh.mNormals[vertex_index];

		vertices.push_back(position.x);
		vertices.push_back(position.y);
		vertices.push_back(position.z);
		vertices.push_back(normal.x);
		vertices.push_back(normal.y);
		vertices.push_back(normal.z);

		if (mesh.mTextureCoords[0]) {
			vertices.push_back(mesh.mTextureCoords[0][vertex_index].x);
			vertices.push_back(1.0f - mesh.mTextureCoords[0][vertex_index].y);
		} else {
			vertices.push_back(0.0f);
			vertices.push_back(0.0f);
		}

		aiVector3D tangent = aiVector3D(1.0f, 0.0f, 0.0f);
		float tangent_handedness = 1.0f;
		const aiVector3D safe_normal = ModelInternal::normalize_safe(normal);

		if (mesh.HasTangentsAndBitangents()) {
			tangent = ModelInternal::normalize_safe(mesh.mTangents[vertex_index]);
			const aiVector3D bitangent = ModelInternal::normalize_safe(mesh.mBitangents[vertex_index]);
			const aiVector3D expected_bitangent = ModelInternal::normalize_safe(ModelInternal::cross3(safe_normal, tangent));
			if (ModelInternal::dot3(expected_bitangent, bitangent) < 0.0f)
				tangent_handedness = -1.0f;
		} else {
			const aiVector3D up(0.0f, 1.0f, 0.0f);
			const aiVector3D right(1.0f, 0.0f, 0.0f);
			tangent = ModelInternal::normalize_safe(std::abs(safe_normal.y) < 0.999f
			        ? ModelInternal::cross3(up, safe_normal)
			        : ModelInternal::cross3(safe_normal, right));
		}

		vertices.push_back(tangent.x);
		vertices.push_back(tangent.y);
		vertices.push_back(tangent.z);
		vertices.push_back(tangent_handedness);
	}

	Bounds compute_mesh_bounds(const aiMesh& mesh) {
		Bounds bounds {};
		if (mesh.mNumVertices == 0)
			return bounds;

		const aiVector3D& first = mesh.mVertices[0];
		bounds.Min = glm::vec3(first.x, first.y, first.z);
		bounds.Max = bounds.Min;

		for (uint32_t vertex_index = 1; vertex_index < mesh.mNumVertices; ++vertex_index) {
			const aiVector3D& position = mesh.mVertices[vertex_index];
			const glm::vec3 point(position.x, position.y, position.z);
			bounds.Min = glm::min(bounds.Min, point);
			bounds.Max = glm::max(bounds.Max, point);
		}

		return bounds;
	}

	void build_mesh_buffers(const aiMesh& mesh, std::vector<float>& vertices, std::vector<uint32_t>& indices) {
		vertices.clear();
		indices.clear();

		vertices.reserve(mesh.mNumVertices * 12);

		for (uint32_t vertex_index = 0; vertex_index < mesh.mNumVertices; ++vertex_index) {
			append_vertex_data(vertices, mesh, vertex_index);
		}

		for (uint32_t face_index = 0; face_index < mesh.mNumFaces; ++face_index) {
			const aiFace& face = mesh.mFaces[face_index];
			if (face.mNumIndices != 3)
				continue;

			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}
	}

	AssetHandle create_runtime_mesh(
	    Model& model,
	    uint32_t mesh_index,
	    const std::vector<float>& vertices,
	    const std::vector<uint32_t>& indices,
	    const OptionalUUID& model_id) {
		const int layout_id = VertexLayout::get_static_mesh_tangent_layout();
		const int heap_id = GeometryHeapRegistry::get_or_create_static_mesh_heap(layout_id);
		GeometryHeap* heap = GeometryHeapRegistry::get_heap(heap_id);

		auto range = heap->allocate(
		    vertices.data(),
		    vertices.size() * sizeof(float),
		    12 * sizeof(float),
		    indices.data(),
		    indices.size());

		AssetHandle mesh_handle = model_id
		    ? AssetFactory::get_instance().create_runtime_asset_with_parent<Mesh>(
		          "ModelMesh_" + std::to_string(mesh_index),
		          *model_id,
		          layout_id,
		          heap_id,
		          range)
		    : AssetFactory::get_instance().create_runtime_asset<Mesh>(
		          "ModelMesh_" + std::to_string(mesh_index),
		          layout_id,
		          heap_id,
		          range);

		auto mesh_asset = AssetManager::get_instance().get_asset<Mesh>(mesh_handle);
		if (mesh_asset)
			mesh_asset->set_owner(&model);

		return mesh_handle;
	}

	void apply_assimp_material_constants(MaterialPBR& material, aiMaterial* ai_material) {
		if (!ai_material)
			return;

		aiColor4D base_color {};
		if (aiGetMaterialColor(ai_material, AI_MATKEY_BASE_COLOR, &base_color) == AI_SUCCESS) {
			material.get_data().PBRParams.BaseColor = Vec3(base_color.r, base_color.g, base_color.b);
		} else {
			aiColor4D diffuse_color {};
			if (aiGetMaterialColor(ai_material, AI_MATKEY_COLOR_DIFFUSE, &diffuse_color) == AI_SUCCESS)
				material.get_data().PBRParams.BaseColor = Vec3(diffuse_color.r, diffuse_color.g, diffuse_color.b);
		}

		float metallic = material.get_data().PBRParams.Metallic;
		if (aiGetMaterialFloat(ai_material, AI_MATKEY_METALLIC_FACTOR, &metallic) == AI_SUCCESS)
			material.get_data().PBRParams.Metallic = metallic;

		float roughness = material.get_data().PBRParams.Roughness;
		if (aiGetMaterialFloat(ai_material, AI_MATKEY_ROUGHNESS_FACTOR, &roughness) == AI_SUCCESS)
			material.get_data().PBRParams.Roughness = roughness;
	}

	AssetHandle create_runtime_material(
	    Model& model,
	    uint32_t mesh_index,
	    const OptionalUUID& model_id,
	    aiMaterial* ai_material,
	    const ImportedTextureHandles& textures) {
		const auto shader_handle = model.get_shader_handle();
		const AssetHandle resolved_shader = shader_handle
		    ? *shader_handle
		    : DefaultAssetCache::get_instance().get_shader();

		const AssetHandle material_handle = model_id
		    ? AssetFactory::get_instance().create_runtime_asset_with_parent<MaterialPBR>(
		          "ModelMat_" + std::to_string(mesh_index),
		          *model_id)
		    : AssetFactory::get_instance().create_runtime_asset<MaterialPBR>(
		          "ModelMat_" + std::to_string(mesh_index));

		auto material = AssetManager::get_instance().get_asset<MaterialPBR>(material_handle);
		if (!material)
			return material_handle;

		apply_assimp_material_constants(*material, ai_material);

		material
		    ->set_albedo(textures.Albedo)
		    .set_normal(textures.Normal)
		    .set_metallic_roughness(textures.MetallicRoughness)
		    .set_ao(textures.Ao)
		    .set_emissive(textures.Emissive)
		    .set_shader(resolved_shader)
		    .set_owner(&model);
		material->resolve();

		return material_handle;
	}

}

void Model::load_from_assimp() {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(m_path.string(), k_assimp_load_flags);

	if (!scene || !scene->mRootNode) {
		ORG_CORE_ERROR("Assimp failed to load '{}': {}", m_path.string(), importer.GetErrorString());
		ModelInternal::make_fallback_model(m_nodes, m_root_node);
		return;
	}

	const auto model_id = AssetManager::get_instance().get_uuid(this);
	const std::filesystem::path parent_path = m_path.parent_path();

	m_assimp_mesh_to_sub_mesh.assign(scene->mNumMeshes, -1);
	m_sub_meshes.reserve(scene->mNumMeshes);

	std::vector<float> vertices;
	std::vector<uint32_t> indices;

	for (uint32_t mesh_index = 0; mesh_index < scene->mNumMeshes; ++mesh_index) {
		aiMesh* ai_mesh = scene->mMeshes[mesh_index];
		if (!ai_mesh || ai_mesh->mNumVertices == 0)
			continue;
		if (ai_mesh->mMaterialIndex >= scene->mNumMaterials)
			continue;

		build_mesh_buffers(*ai_mesh, vertices, indices);

		const AssetHandle mesh_handle = create_runtime_mesh(*this, mesh_index, vertices, indices, model_id);
		if (auto* mesh_asset = AssetManager::get_instance().get_asset<Mesh>(mesh_handle)) {
			const Bounds mesh_bounds = compute_mesh_bounds(*ai_mesh);
			mesh_asset->set_bounds(mesh_bounds.Min, mesh_bounds.Max);
		}
		aiMaterial* ai_material = scene->mMaterials[ai_mesh->mMaterialIndex];
		auto load_texture = [&](aiTextureType assimp_texture_type, TextureType engine_texture_type, const char* debug_prefix) {
			return try_load_material_texture(
			    scene,
			    ai_material,
			    assimp_texture_type,
			    engine_texture_type,
			    parent_path,
			    std::string(debug_prefix) + std::to_string(mesh_index));
		};

		ImportedTextureHandles textures;
		textures.Albedo = load_texture(aiTextureType_BASE_COLOR, TextureType::Albedo, "ModelTex_Albedo_");
		if (!textures.Albedo)
			textures.Albedo = load_texture(aiTextureType_DIFFUSE, TextureType::Albedo, "ModelTex_Albedo_");

		textures.Normal = load_texture(aiTextureType_NORMALS, TextureType::Normal, "ModelTex_Normal_");
		if (!textures.Normal)
			textures.Normal = load_texture(aiTextureType_HEIGHT, TextureType::Normal, "ModelTex_Normal_");

		textures.MetallicRoughness = load_texture(aiTextureType_METALNESS, TextureType::MetallicRoughness, "ModelTex_MetallicRoughness_");
		if (!textures.MetallicRoughness) {
			textures.MetallicRoughness = load_texture(
			    aiTextureType_DIFFUSE_ROUGHNESS,
			    TextureType::MetallicRoughness,
			    "ModelTex_MetallicRoughness_");
		}

		textures.Ao = load_texture(aiTextureType_AMBIENT_OCCLUSION, TextureType::Ao, "ModelTex_AO_");
		textures.Emissive = load_texture(aiTextureType_EMISSIVE, TextureType::Emissive, "ModelTex_Emissive_");

		const AssetHandle material_handle = create_runtime_material(*this, mesh_index, model_id, ai_material, textures);

		m_assimp_mesh_to_sub_mesh[mesh_index] = (int)m_sub_meshes.size();
		m_sub_meshes.push_back({ mesh_handle, material_handle });
	}

	m_root_node = process_node(scene->mRootNode, -1);
	if (m_nodes.empty() || m_root_node < 0 || m_root_node >= (int)m_nodes.size())
		ModelInternal::make_fallback_model(m_nodes, m_root_node);
}

}
