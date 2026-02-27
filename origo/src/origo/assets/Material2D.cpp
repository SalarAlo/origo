#include "origo/assets/Material2D.h"

#include "origo/assets/AssetManager.h"
#include "origo/assets/DefaultAssetCache.h"
#include "origo/assets/Texture2D.h"
#include "origo/assets/UniformList.hpp"

#include "origo/core/Logger.h"

namespace Origo {

Material2D::Material2D() {
	m_shader = DefaultAssetCache::get_instance().get_shader();
	m_albedo = DefaultAssetCache::get_instance().get_texture();
}

Material2D::Material2D(AssetHandle shader, OptionalAssetHandle texture)
    : m_uniform_list()
    , m_shader(shader)
    , m_albedo(texture) {
	if (!m_albedo.has_value())
		return;

	auto albedo_ptr { AssetManager::get_instance().get_asset<Texture2D>(*m_albedo) };
	if (albedo_ptr->get_texture_type() != TextureType::Albedo) {
		ORG_CORE_ERROR(
		    "Material::Material: Expected a Texture Type of Albedo. Received a Texture Type of {}",
		    magic_enum::enum_name(albedo_ptr->get_texture_type()));
	}
}

void Material2D::bind() {
	auto& am { AssetManager::get_instance() };
	auto shader { am.get_asset<Shader>(*m_shader) };
	shader->use_program();
	shader->set_uniform("u_Color", m_color);
	m_uniform_list.upload_all(shader);

	if (m_albedo.has_value()) {
		auto albedo { am.get_asset<Texture2D>(*m_albedo) };
		albedo->bind(*m_shader);
	}
}

void Material2D::write_model(const glm::mat4& modelMatrix) {
	if (!m_shader.has_value()) {
		ORG_TRACE("Material2D::WriteModel: Can not Write Model if no shader exists");
		return;
	}

	auto shader { AssetManager::get_instance().get_asset<Shader>(*m_shader) };
	shader->set_uniform("u_ModelMatrix", modelMatrix);
}

void Material2D::resolve() {
	auto& am = AssetManager::get_instance();
	auto& ac { DefaultAssetCache::get_instance() };

	m_shader = !m_shader_uuid.has_value() ? ac.get_shader() : am.get_handle_by_uuid(*m_shader_uuid);
	if (!m_shader.has_value()) {
		m_shader = ac.get_shader();
	}

	m_albedo = !m_albedo_uuid.has_value() ? ac.get_texture() : am.get_handle_by_uuid(*m_albedo_uuid);
	if (!m_albedo.has_value()) {
		m_albedo = ac.get_texture();
	}

	set_textured();
}

void Material2D::set_color(Vec3 color) {
	m_color = color;
	m_uniform_list.add_uniform("m_Color", color);
}

void Material2D::set_textured() {
	set_uniform("u_UseTexture", true);
	set_uniform("u_UseLight", true);
}

}
