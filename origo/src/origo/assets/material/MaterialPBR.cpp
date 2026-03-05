#include "origo/assets/material/MaterialPBR.h"

#include "origo/assets/AssetManager.h"
#include "origo/assets/Texture2D.h"
#include "origo/assets/UniformList.hpp"

#include "origo/assets/material/MaterialPBRSource.h"

namespace Origo {

void MaterialPBR::bind() {
	if (!m_shader)
		return;

	m_shader->use_program();

	m_shader->set_uniform("u_Color", m_data.PBRParams.BaseColor);

	if (m_albedo && m_data.ShaderHandle)
		m_albedo->bind(*m_data.ShaderHandle);

	m_uniform_list.upload_all(m_shader);
}

void MaterialPBR::write_model(const glm::mat4& modelMatrix) {
	if (!m_shader)
		return;

	m_shader->set_uniform("u_ModelMatrix", modelMatrix);
}

void MaterialPBR::resolve() {
	auto& am = AssetManager::get_instance();

	if (!m_data.ShaderHandle && m_uuid_deps.Shader) {
		auto shader_handle = am.get_handle_by_uuid(*m_uuid_deps.Shader);
		if (shader_handle)
			m_data.ShaderHandle = *shader_handle;
	}

	if (!m_data.PBRTexs.Albedo && m_uuid_deps.Albedo) {
		auto albedo_handle = am.get_handle_by_uuid(*m_uuid_deps.Albedo);
		if (albedo_handle)
			m_data.PBRTexs.Albedo = *albedo_handle;
	}

	m_shader = nullptr;
	m_albedo = nullptr;

	resolve_shader();
	resolve_albedo();

	if (!m_shader)
		ORG_CORE_ERROR("Material resolve failed: missing shader");
}

void MaterialPBR::resolve_shader() {
	if (!m_data.ShaderHandle)
		return;

	auto& am = AssetManager::get_instance();
	m_shader = am.get_asset<Shader>(*m_data.ShaderHandle);
}

MaterialPBR& MaterialPBR::set_shader(const OptionalAssetHandle& shader) {
	if (m_data.ShaderHandle == shader)
		return *this;

	m_data.ShaderHandle = shader;
	m_shader = nullptr;
	resolve_shader();
	return *this;
}

MaterialPBR& MaterialPBR::set_albedo(const OptionalAssetHandle& albedo) {
	if (m_data.PBRTexs.Albedo == albedo)
		return *this;

	m_data.PBRTexs.Albedo = albedo;
	m_albedo = nullptr;
	resolve_albedo();
	return *this;
}

void MaterialPBR::resolve_albedo() {
	if (!m_data.PBRTexs.Albedo)
		return;

	auto& am = AssetManager::get_instance();
	m_albedo = am.get_asset<Texture2D>(*m_data.PBRTexs.Albedo);
}

void MaterialPBR::make_textured_material() {
	set_uniform("u_UseTexture", true);
	set_uniform("u_UseLight", true);
}

}
