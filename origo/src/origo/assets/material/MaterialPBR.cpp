#include "origo/assets/material/MaterialPBR.h"

#include "origo/assets/AssetManager.h"
#include "origo/assets/DefaultAssetCache.h"
#include "origo/assets/Texture2D.h"
#include "origo/assets/UniformList.hpp"

#include "origo/assets/material/MaterialPBRSource.h"

namespace Origo {

MaterialPBR::MaterialPBR()
    : Material(DefaultAssetCache::get_instance().get_shader()) {
}

void MaterialPBR::bind() {
	if (!m_shader)
		return;

	bind_shader_and_upload_uniforms();

	m_shader->set_uniform("u_base_color_factor", m_data.PBRParams.BaseColor);
	m_shader->set_uniform("u_metallic_factor", m_data.PBRParams.Metallic);
	m_shader->set_uniform("u_roughness_factor", m_data.PBRParams.Roughness);
	m_shader->set_uniform("u_ao_factor", m_data.PBRParams.AO);
	m_shader->set_uniform("u_unlit", m_data.PBRParams.Unlit);

	if (m_albedo && m_shader_handle)
		m_albedo->bind(*m_shader_handle);
	if (m_normal && m_shader_handle)
		m_normal->bind(*m_shader_handle);
	if (m_metallic_roughness && m_shader_handle)
		m_metallic_roughness->bind(*m_shader_handle);
	if (m_ao && m_shader_handle)
		m_ao->bind(*m_shader_handle);
	if (m_emissive && m_shader_handle)
		m_emissive->bind(*m_shader_handle);

	m_shader->set_uniform("u_has_albedo_map", m_albedo != nullptr);
	m_shader->set_uniform("u_has_normal_map", m_normal != nullptr);
	m_shader->set_uniform("u_has_metallic_roughness_map", m_metallic_roughness != nullptr);
	m_shader->set_uniform("u_has_ao_map", m_ao != nullptr);
	m_shader->set_uniform("u_has_emissive_map", m_emissive != nullptr);
}

void MaterialPBR::resolve() {
	auto& am = AssetManager::get_instance();

	if (!m_data.PBRTexs.Albedo && m_uuid_deps.Albedo) {
		auto albedo_handle = am.get_handle_by_uuid(*m_uuid_deps.Albedo);
		if (albedo_handle)
			m_data.PBRTexs.Albedo = *albedo_handle;
	}
	if (!m_data.PBRTexs.Normal && m_uuid_deps.Normal) {
		auto normal_handle = am.get_handle_by_uuid(*m_uuid_deps.Normal);
		if (normal_handle)
			m_data.PBRTexs.Normal = *normal_handle;
	}
	if (!m_data.PBRTexs.MetallicRoughness && m_uuid_deps.MetallicRoughness) {
		auto metallic_roughness_handle = am.get_handle_by_uuid(*m_uuid_deps.MetallicRoughness);
		if (metallic_roughness_handle)
			m_data.PBRTexs.MetallicRoughness = *metallic_roughness_handle;
	}
	if (!m_data.PBRTexs.Ao && m_uuid_deps.Ao) {
		auto ao_handle = am.get_handle_by_uuid(*m_uuid_deps.Ao);
		if (ao_handle)
			m_data.PBRTexs.Ao = *ao_handle;
	}
	if (!m_data.PBRTexs.Emissive && m_uuid_deps.Emissive) {
		auto emissive_handle = am.get_handle_by_uuid(*m_uuid_deps.Emissive);
		if (emissive_handle)
			m_data.PBRTexs.Emissive = *emissive_handle;
	}

	m_albedo = nullptr;
	m_normal = nullptr;
	m_metallic_roughness = nullptr;
	m_ao = nullptr;
	m_emissive = nullptr;

	resolve_material_base();
	resolve_albedo();
	resolve_normal();
	resolve_metallic_roughness();
	resolve_ao();
	resolve_emissive();

	if (!m_shader)
		ORG_CORE_ERROR("Material resolve failed: missing shader");
}

MaterialPBR& MaterialPBR::set_albedo(const OptionalAssetHandle& albedo) {
	if (m_data.PBRTexs.Albedo == albedo)
		return *this;

	m_data.PBRTexs.Albedo = albedo;
	m_albedo = nullptr;
	resolve_albedo();
	return *this;
}

MaterialPBR& MaterialPBR::set_normal(const OptionalAssetHandle& normal) {
	if (m_data.PBRTexs.Normal == normal)
		return *this;

	m_data.PBRTexs.Normal = normal;
	m_normal = nullptr;
	resolve_normal();
	return *this;
}

MaterialPBR& MaterialPBR::set_metallic_roughness(const OptionalAssetHandle& metallicRoughness) {
	if (m_data.PBRTexs.MetallicRoughness == metallicRoughness)
		return *this;

	m_data.PBRTexs.MetallicRoughness = metallicRoughness;
	m_metallic_roughness = nullptr;
	resolve_metallic_roughness();
	return *this;
}

MaterialPBR& MaterialPBR::set_ao(const OptionalAssetHandle& ao) {
	if (m_data.PBRTexs.Ao == ao)
		return *this;

	m_data.PBRTexs.Ao = ao;
	m_ao = nullptr;
	resolve_ao();
	return *this;
}

MaterialPBR& MaterialPBR::set_emissive(const OptionalAssetHandle& emissive) {
	if (m_data.PBRTexs.Emissive == emissive)
		return *this;

	m_data.PBRTexs.Emissive = emissive;
	m_emissive = nullptr;
	resolve_emissive();
	return *this;
}

MaterialPBR& MaterialPBR::set_unlit(bool unlit) {
	m_data.PBRParams.Unlit = unlit;
	return *this;
}

void MaterialPBR::resolve_albedo() {
	if (!m_data.PBRTexs.Albedo)
		return;

	auto& am = AssetManager::get_instance();
	m_albedo = am.get_asset<Texture2D>(*m_data.PBRTexs.Albedo);
	if (m_albedo)
		m_albedo->set_texture_type(TextureType::Albedo);
}

void MaterialPBR::resolve_normal() {
	if (!m_data.PBRTexs.Normal)
		return;

	auto& am = AssetManager::get_instance();
	m_normal = am.get_asset<Texture2D>(*m_data.PBRTexs.Normal);
	if (m_normal)
		m_normal->set_texture_type(TextureType::Normal);
}

void MaterialPBR::resolve_metallic_roughness() {
	if (!m_data.PBRTexs.MetallicRoughness)
		return;

	auto& am = AssetManager::get_instance();
	m_metallic_roughness = am.get_asset<Texture2D>(*m_data.PBRTexs.MetallicRoughness);
	if (m_metallic_roughness)
		m_metallic_roughness->set_texture_type(TextureType::MetallicRoughness);
}

void MaterialPBR::resolve_ao() {
	if (!m_data.PBRTexs.Ao)
		return;

	auto& am = AssetManager::get_instance();
	m_ao = am.get_asset<Texture2D>(*m_data.PBRTexs.Ao);
	if (m_ao)
		m_ao->set_texture_type(TextureType::Ao);
}

void MaterialPBR::resolve_emissive() {
	if (!m_data.PBRTexs.Emissive)
		return;

	auto& am = AssetManager::get_instance();
	m_emissive = am.get_asset<Texture2D>(*m_data.PBRTexs.Emissive);
	if (m_emissive)
		m_emissive->set_texture_type(TextureType::Emissive);
}

}
