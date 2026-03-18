#include "DefaultAssetCache.h"

#include "origo/assets/AssetFactory.h"
#include "origo/assets/AssetManager.h"
#include "origo/assets/Shader.h"
#include "origo/assets/Texture2D.h"

#include "origo/assets/material/MaterialPBR.h"

#include "origo/core/PathContext.h"

#include "origo/core/Typedefs.h"

namespace Origo {

void DefaultAssetCache::create_all_defaults() {
	get_texture();
	get_shader();
	get_material();
	get_terrain_material();
	get_outline_material();
	get_particle_material();
	get_debug_material();
}

AssetHandle DefaultAssetCache::get_shader() {
	if (m_shader.has_value())
		return *m_shader;

	m_shader = AssetFactory::get_instance().get_instance().create_synthetic_asset<Shader>(
	    "Default Shader",
	    UUID::from_hash("ENGINE_DEFAULT_SHADER"));

	auto shader = AssetManager::get_instance().get_asset<Shader>(*m_shader);
	shader->set_source(make_scope<ShaderSourceFile>(
	    PathContextService::get_instance().editor().fallback_root() / "shaders" / "default_shader.glsl"));
	shader->resolve();

	return *m_shader;
}

AssetHandle DefaultAssetCache::get_texture() {
	if (m_texture.has_value())
		return *m_texture;

	std::vector<unsigned char> white_pixel_rgba = { 255, 255, 255, 255 };

	m_texture = AssetFactory::get_instance().get_instance().create_synthetic_asset<Texture2D>(
	    "Default White Texture",
	    UUID::from_arbitrary_string("DEFAULT_TEXTURE_2D"));

	auto tex = AssetManager::get_instance().get_asset<Texture2D>(*m_texture);
	tex->set_source(make_scope<TextureSourceRaw>(
	    1, 1, 4, std::move(white_pixel_rgba), false));
	tex->load();

	return *m_texture;
}

AssetHandle DefaultAssetCache::get_material() {
	if (m_material.has_value())
		return *m_material;

	m_material = AssetFactory::get_instance().get_instance().create_synthetic_asset<MaterialPBR>(
	    "Default Material",
	    UUID::from_arbitrary_string("DEFAULT_MATERIAL_2D"));

	auto material = AssetManager::get_instance().get_asset<MaterialPBR>(*m_material);
	material->set_shader(get_shader());
	material->set_albedo(get_texture());

	material->resolve();

	return *m_material;
}

AssetHandle DefaultAssetCache::get_terrain_material() {
	if (m_terrain_material.has_value())
		return *m_terrain_material;

	m_terrain_material = AssetFactory::get_instance().get_instance().create_synthetic_asset<MaterialPBR>(
	    "Terrain Material",
	    UUID::from_arbitrary_string("DEFAULT_TERRAIN_MATERIAL_2D"));

	auto material = AssetManager::get_instance().get_asset<MaterialPBR>(*m_terrain_material);
	material->set_shader(get_shader());
	material->set_albedo(get_texture());
	material->get_data().PBRParams.BaseColor = Vec3 { 0.33f, 0.53f, 0.24f };
	material->get_data().PBRParams.Metallic = 0.02f;
	material->get_data().PBRParams.Roughness = 0.92f;
	material->get_data().PBRParams.AO = 1.0f;
	material->resolve();

	return *m_terrain_material;
}

AssetHandle DefaultAssetCache::get_debug_material() {
	constexpr Vec3 debug_emission_particle_color = Vec3 { .1f, .9f, .05f };

	if (m_particle_emission_debug_material.has_value())
		return *m_particle_emission_debug_material;

	m_particle_emission_debug_material = AssetFactory::get_instance().get_instance().create_synthetic_asset<MaterialPBR>(
	    "Emission Shape Material",
	    UUID::from_arbitrary_string("DEFAULT_DEBUG_PARTICLE_MATERIAL_2D"));

	auto material = AssetManager::get_instance().get_asset<MaterialPBR>(*m_particle_emission_debug_material);
	material->set_shader(get_shader());
	material->set_albedo(get_texture());
	material->get_data().PBRParams.BaseColor = debug_emission_particle_color;
	material->set_unlit(true);

	material->resolve();

	return *m_particle_emission_debug_material;
}

Origo::AssetHandle DefaultAssetCache::get_outline_material() {
	constexpr Vec3 orange_color = Vec3 { 1.0f, (125.0f / 255.0f), 0.0f };

	if (m_outline_material.has_value())
		return *m_outline_material;

	m_outline_material = AssetFactory::get_instance().get_instance().create_synthetic_asset<MaterialPBR>(
	    "Outline Material",
	    UUID::from_arbitrary_string("DEFAULT_MATERIAL_OUTLINE_2D"));

	auto material = AssetManager::get_instance().get_asset<MaterialPBR>(*m_outline_material);

	material->set_shader(get_shader());
	material->get_data().PBRParams.BaseColor = orange_color;
	material->set_unlit(true);

	material->resolve();

	return *m_outline_material;
}

AssetHandle DefaultAssetCache::get_particle_material() {
	constexpr Vec3 particle_color = Vec3 { 1.0f, 0.0f, 0.0f };

	if (m_particle_material.has_value())
		return *m_particle_material;

	m_particle_material = AssetFactory::get_instance().get_instance().create_synthetic_asset<MaterialPBR>(
	    "Particle Material",
	    UUID::from_arbitrary_string("DEFAULT_PARTICLE_MATERIAL_2D"));

	auto material = AssetManager::get_instance().get_asset<MaterialPBR>(*m_particle_material);

	material->set_shader(get_shader());
	material->set_albedo(get_texture());

	material->get_data().PBRParams.BaseColor = particle_color;
	material->set_unlit(false);

	material->resolve();

	return *m_particle_material;
}

}
