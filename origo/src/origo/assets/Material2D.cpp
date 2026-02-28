#include "origo/assets/Material2D.h"

#include "origo/assets/AssetManager.h"
#include "origo/assets/Material2DSource.h"
#include "origo/assets/Texture2D.h"
#include "origo/assets/UniformList.hpp"

#include "origo/core/Logger.h"
#include "origo/core/Typedefs.h"

namespace Origo {

void Material2D::bind() {
	if (!m_resolved) {
		ORG_CORE_ERROR("Can not bound unresolved material. call resolve after set_source.");
		return;
	}

	m_shader->use_program();
	m_shader->set_uniform("u_Color", m_color);

	m_uniform_list.upload_all(m_shader);

	m_albedo->bind(m_data->ShaderHandle);
}

void Material2D::write_model(const glm::mat4& modelMatrix) {
	if (!m_resolved) {
		ORG_CORE_ERROR("Can not bound unresolved material. call resolve after set_source.");
		return;
	}

	m_shader->set_uniform("u_ModelMatrix", modelMatrix);
}

void Material2D::resolve() {
	if (!m_source) {
		ORG_CORE_ERROR("Can not resolve material with unset source");
		return;
	}

	auto& am = AssetManager::get_instance();

	m_resolved = false;
	m_shader = nullptr;
	m_albedo = nullptr;

	m_data = m_source->get_material_data();

	m_shader = am.get_asset<Shader>(m_data->ShaderHandle);
	m_albedo = am.get_asset<Texture2D>(m_data->AlbedoHandle);

	if (!m_shader || !m_albedo) {
		ORG_CORE_ERROR("Material resolve failed");
		m_resolved = false;
		return;
	}

	m_resolved = true;
}

void Material2D::set_color(Vec3 color) {
	m_color = color;
}

void Material2D::make_textured_material() {
	set_uniform("u_UseTexture", true);
	set_uniform("u_UseLight", true);
}

void Material2D::set_source(Scope<Material2DSource> source) {
	m_source = std::move(source);

	m_resolved = false;
	m_shader = nullptr;
	m_albedo = nullptr;
	m_data.reset();
}

Material2DSource* Material2D::get_source() const {
	return m_source.get();
}

std::optional<MaterialData> Material2D::get_data() const {
	return m_data;
}

}
