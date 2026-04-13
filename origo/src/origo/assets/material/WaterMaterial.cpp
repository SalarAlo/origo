#include "origo/assets/material/WaterMaterial.h"

#include <chrono>

#include "origo/assets/AssetManager.h"
#include "origo/assets/DefaultAssetCache.h"

namespace Origo {

WaterMaterial::WaterMaterial()
    : Material(DefaultAssetCache::get_instance().get_water_shader()) {
}

void WaterMaterial::resolve_texture(Texture2D*& destination, const OptionalAssetHandle& handle) {
	destination = nullptr;
	if (!handle)
		return;

	destination = AssetManager::get_instance().get_asset<Texture2D>(*handle);
}

void WaterMaterial::resolve() {
	resolve_material_base();
	resolve_texture(m_wave_a, m_wave_a_handle);
	resolve_texture(m_wave_b, m_wave_b_handle);
}

void WaterMaterial::bind() {
	if (!m_shader)
		return;

	bind_shader_and_upload_uniforms();

	static const auto start_time = std::chrono::steady_clock::now();
	const auto now = std::chrono::steady_clock::now();
	const std::chrono::duration<float> elapsed = now - start_time;
	m_shader->set_uniform("u_time", elapsed.count());

	if (m_wave_a) {
		glActiveTexture(GL_TEXTURE0 + static_cast<int>(TextureSlot::WaveA));
		glBindTexture(GL_TEXTURE_2D, m_wave_a->get_renderer_id());
		m_shader->set_uniform("u_wave_normal_a", static_cast<int>(TextureSlot::WaveA));
	}

	if (m_wave_b) {
		glActiveTexture(GL_TEXTURE0 + static_cast<int>(TextureSlot::WaveB));
		glBindTexture(GL_TEXTURE_2D, m_wave_b->get_renderer_id());
		m_shader->set_uniform("u_wave_normal_b", static_cast<int>(TextureSlot::WaveB));
	}
}

WaterMaterial& WaterMaterial::set_wave_a(const OptionalAssetHandle& handle) {
	m_wave_a_handle = handle;
	resolve_texture(m_wave_a, handle);
	return *this;
}

WaterMaterial& WaterMaterial::set_wave_b(const OptionalAssetHandle& handle) {
	m_wave_b_handle = handle;
	resolve_texture(m_wave_b, handle);
	return *this;
}

}
