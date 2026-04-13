#include "origo/assets/material/TerrainMaterial.h"

#include "origo/assets/AssetManager.h"
#include "origo/assets/DefaultAssetCache.h"

namespace Origo {

TerrainMaterial::TerrainMaterial()
    : Material(DefaultAssetCache::get_instance().get_terrain_shader()) {
}

void TerrainMaterial::resolve_texture(Texture2D*& destination, const OptionalAssetHandle& handle) {
	destination = nullptr;
	if (!handle)
		return;

	destination = AssetManager::get_instance().get_asset<Texture2D>(*handle);
	if (!destination)
		return;

	destination->set_wrap_mode(TextureWrapMode::Repeat);
	destination->resolve();
}

void TerrainMaterial::resolve() {
	resolve_material_base();
	resolve_texture(m_ground_albedo, m_ground_albedo_handle);
	resolve_texture(m_ground_normal, m_ground_normal_handle);
	resolve_texture(m_ground_packed, m_ground_packed_handle);
	resolve_texture(m_rock_albedo, m_rock_albedo_handle);
	resolve_texture(m_rock_normal, m_rock_normal_handle);
	resolve_texture(m_rock_packed, m_rock_packed_handle);
	resolve_texture(m_sand_albedo, m_sand_albedo_handle);
	resolve_texture(m_sand_normal, m_sand_normal_handle);
	resolve_texture(m_sand_packed, m_sand_packed_handle);
	resolve_texture(m_snow_albedo, m_snow_albedo_handle);
	resolve_texture(m_snow_normal, m_snow_normal_handle);
	resolve_texture(m_snow_packed, m_snow_packed_handle);
	resolve_texture(m_micro_normal, m_micro_normal_handle);
}

void TerrainMaterial::bind_texture(Texture2D* texture, TextureSlot slot, const char* uniform_name) {
	if (!texture || !m_shader)
		return;

	if (texture->get_renderer_id() == 0) {
		texture->set_wrap_mode(TextureWrapMode::Repeat);
		texture->resolve();
	}

	const int slot_index = static_cast<int>(slot);
	glActiveTexture(GL_TEXTURE0 + slot_index);
	glBindTexture(GL_TEXTURE_2D, texture->get_renderer_id());
	m_shader->set_uniform(uniform_name, slot_index);
}

void TerrainMaterial::bind() {
	if (!m_shader)
		return;

	bind_shader_and_upload_uniforms();

	bind_texture(m_ground_albedo, TextureSlot::GroundAlbedo, "u_ground_albedo");
	bind_texture(m_ground_normal, TextureSlot::GroundNormal, "u_ground_normal");
	bind_texture(m_ground_packed, TextureSlot::GroundPacked, "u_ground_packed");
	bind_texture(m_rock_albedo, TextureSlot::RockAlbedo, "u_rock_albedo");
	bind_texture(m_rock_normal, TextureSlot::RockNormal, "u_rock_normal");
	bind_texture(m_rock_packed, TextureSlot::RockPacked, "u_rock_packed");
	bind_texture(m_sand_albedo, TextureSlot::SandAlbedo, "u_sand_albedo");
	bind_texture(m_sand_normal, TextureSlot::SandNormal, "u_sand_normal");
	bind_texture(m_sand_packed, TextureSlot::SandPacked, "u_sand_packed");
	bind_texture(m_snow_albedo, TextureSlot::SnowAlbedo, "u_snow_albedo");
	bind_texture(m_snow_normal, TextureSlot::SnowNormal, "u_snow_normal");
	bind_texture(m_snow_packed, TextureSlot::SnowPacked, "u_snow_packed");
	bind_texture(m_micro_normal, TextureSlot::MicroNormal, "u_micro_normal");
}

TerrainMaterial& TerrainMaterial::set_ground_albedo(const OptionalAssetHandle& handle) {
	m_ground_albedo_handle = handle;
	resolve_texture(m_ground_albedo, handle);
	return *this;
}

TerrainMaterial& TerrainMaterial::set_ground_normal(const OptionalAssetHandle& handle) {
	m_ground_normal_handle = handle;
	resolve_texture(m_ground_normal, handle);
	return *this;
}

TerrainMaterial& TerrainMaterial::set_ground_packed(const OptionalAssetHandle& handle) {
	m_ground_packed_handle = handle;
	resolve_texture(m_ground_packed, handle);
	return *this;
}

TerrainMaterial& TerrainMaterial::set_rock_albedo(const OptionalAssetHandle& handle) {
	m_rock_albedo_handle = handle;
	resolve_texture(m_rock_albedo, handle);
	return *this;
}

TerrainMaterial& TerrainMaterial::set_rock_normal(const OptionalAssetHandle& handle) {
	m_rock_normal_handle = handle;
	resolve_texture(m_rock_normal, handle);
	return *this;
}

TerrainMaterial& TerrainMaterial::set_rock_packed(const OptionalAssetHandle& handle) {
	m_rock_packed_handle = handle;
	resolve_texture(m_rock_packed, handle);
	return *this;
}

TerrainMaterial& TerrainMaterial::set_sand_albedo(const OptionalAssetHandle& handle) {
	m_sand_albedo_handle = handle;
	resolve_texture(m_sand_albedo, handle);
	return *this;
}

TerrainMaterial& TerrainMaterial::set_sand_normal(const OptionalAssetHandle& handle) {
	m_sand_normal_handle = handle;
	resolve_texture(m_sand_normal, handle);
	return *this;
}

TerrainMaterial& TerrainMaterial::set_sand_packed(const OptionalAssetHandle& handle) {
	m_sand_packed_handle = handle;
	resolve_texture(m_sand_packed, handle);
	return *this;
}

TerrainMaterial& TerrainMaterial::set_snow_albedo(const OptionalAssetHandle& handle) {
	m_snow_albedo_handle = handle;
	resolve_texture(m_snow_albedo, handle);
	return *this;
}

TerrainMaterial& TerrainMaterial::set_snow_normal(const OptionalAssetHandle& handle) {
	m_snow_normal_handle = handle;
	resolve_texture(m_snow_normal, handle);
	return *this;
}

TerrainMaterial& TerrainMaterial::set_snow_packed(const OptionalAssetHandle& handle) {
	m_snow_packed_handle = handle;
	resolve_texture(m_snow_packed, handle);
	return *this;
}

TerrainMaterial& TerrainMaterial::set_micro_normal(const OptionalAssetHandle& handle) {
	m_micro_normal_handle = handle;
	resolve_texture(m_micro_normal, handle);
	return *this;
}

}
