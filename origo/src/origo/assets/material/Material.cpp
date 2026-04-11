#include "origo/assets/material/Material.h"

#include "origo/assets/AssetManager.h"

namespace Origo {

void Material::write_model(const glm::mat4& modelMatrix) {
	if (!m_shader)
		return;

	m_shader->set_uniform("u_model_matrix", modelMatrix);
}

Material& Material::set_shader(const OptionalAssetHandle& shader) {
	if (m_shader_handle == shader)
		return *this;

	m_shader_handle = shader;
	m_shader = nullptr;
	resolve_shader();
	return *this;
}

void Material::bind_shader_and_upload_uniforms() {
	if (!m_shader)
		return;

	m_shader->use_program();
	m_uniform_list.upload_all(m_shader);
}

void Material::resolve_material_base() {
	auto& am = AssetManager::get_instance();

	if (!m_shader_handle && m_uuid_deps.Shader) {
		auto shader_handle = am.get_handle_by_uuid(*m_uuid_deps.Shader);
		if (shader_handle)
			m_shader_handle = *shader_handle;
	}

	m_shader = nullptr;
	resolve_shader();
}

void Material::resolve_shader() {
	if (!m_shader_handle)
		return;

	auto& am = AssetManager::get_instance();
	m_shader = am.get_asset<Shader>(*m_shader_handle);
}

}
