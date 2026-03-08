#include "Model.h"

#include "ModelInternal.h"

#include "origo/assets/AssetManager.h"
#include "origo/assets/DefaultAssetCache.h"

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
}

void Model::clear() {
	m_sub_meshes.clear();
	m_nodes.clear();
	m_assimp_mesh_to_sub_mesh.clear();
	m_root_node = -1;
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
