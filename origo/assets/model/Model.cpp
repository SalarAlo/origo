#include "Model.h"

#include "ModelInternal.h"

#include "origo/assets/AssetManager.h"
#include "origo/assets/DefaultAssetCache.h"
#include "origo/assets/Mesh.h"

namespace Origo {

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
		ModelInternal::make_fallback_model(m_nodes, m_root_node);
		return;
	}

	clear();
	load_from_assimp();

	if (m_nodes.empty() || m_root_node < 0 || m_root_node >= (int)m_nodes.size()) {
		ORG_CORE_WARN("Model::Load: invalid node graph produced for '{}', using fallback", m_path.string());
		ModelInternal::make_fallback_model(m_nodes, m_root_node);
	}

	compute_bounds();
}

void Model::clear() {
	m_sub_meshes.clear();
	m_nodes.clear();
	m_assimp_mesh_to_sub_mesh.clear();
	m_root_node = -1;
	m_bounds_min = glm::vec3(-0.5f);
	m_bounds_max = glm::vec3(0.5f);
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

void Model::compute_bounds() {
	if (m_root_node < 0 || m_root_node >= static_cast<int>(m_nodes.size()) || m_sub_meshes.empty()) {
		m_bounds_min = glm::vec3(-0.5f);
		m_bounds_max = glm::vec3(0.5f);
		return;
	}

	auto& asset_manager = AssetManager::get_instance();
	std::vector<glm::mat4> world_matrices(m_nodes.size(), glm::mat4(1.0f));
	world_matrices[m_root_node] = m_nodes[m_root_node].LocalTransform;

	bool has_bounds = false;
	glm::vec3 min_bounds(0.0f);
	glm::vec3 max_bounds(0.0f);

	for (size_t i = 0; i < m_nodes.size(); ++i) {
		const auto& node = m_nodes[i];
		if (node.Parent != -1)
			world_matrices[i] = world_matrices[node.Parent] * node.LocalTransform;

		if (node.SubMeshIndex < 0 || node.SubMeshIndex >= static_cast<int>(m_sub_meshes.size()))
			continue;

		auto* mesh = asset_manager.get_asset<Mesh>(m_sub_meshes[node.SubMeshIndex].Mesh);
		if (!mesh)
			continue;

		for (int corner = 0; corner < 8; ++corner) {
			const glm::vec3 point(
			    (corner & 1) ? mesh->BoundsMax.x : mesh->BoundsMin.x,
			    (corner & 2) ? mesh->BoundsMax.y : mesh->BoundsMin.y,
			    (corner & 4) ? mesh->BoundsMax.z : mesh->BoundsMin.z);
			const glm::vec3 world_point = glm::vec3(world_matrices[i] * glm::vec4(point, 1.0f));

			if (!has_bounds) {
				min_bounds = world_point;
				max_bounds = world_point;
				has_bounds = true;
				continue;
			}

			min_bounds = glm::min(min_bounds, world_point);
			max_bounds = glm::max(max_bounds, world_point);
		}
	}

	if (!has_bounds) {
		m_bounds_min = glm::vec3(-0.5f);
		m_bounds_max = glm::vec3(0.5f);
		return;
	}

	m_bounds_min = min_bounds;
	m_bounds_max = max_bounds;
}
}
