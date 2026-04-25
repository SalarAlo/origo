#include "DefaultAssetCache.h"

#include "origo/assets/AssetFactory.h"
#include "origo/assets/AssetManager.h"
#include "origo/assets/Shader.h"
#include "origo/assets/Texture2D.h"

#include "origo/assets/material/MaterialColor.h"
#include "origo/assets/material/MaterialPBR.h"

#include "origo/core/PathContext.h"
#include "origo/core/Typedefs.h"

namespace Origo {
namespace {

unsigned char to_byte(float value) {
	return static_cast<unsigned char>(glm::clamp(value, 0.0f, 1.0f) * 255.0f);
}

float tile_noise(int x, int y, int seed) {
	const float value = std::sin(
	    static_cast<float>(x + seed * 11) * 12.9898f
	    + static_cast<float>(y + seed * 17) * 78.233f) * 43758.5453f;
	return glm::fract(value);
}

AssetHandle create_runtime_texture(
    std::string_view name,
    const UUID& id,
    int width,
    int height,
    std::vector<unsigned char>&& pixels,
    TextureWrapMode wrap_mode) {
	auto handle = AssetFactory::get_instance().create_synthetic_asset<Texture2D>(
	    std::string(name),
	    id);

	auto* texture = AssetManager::get_instance().get_asset<Texture2D>(handle);
	texture->set_source(make_scope<TextureSourceRaw>(width, height, 4, std::move(pixels), true));
	texture->set_wrap_mode(wrap_mode);
	texture->load();
	return handle;
}

std::vector<unsigned char> build_albedo_pixels(
    int width,
    int height,
    const glm::vec3& base_a,
    const glm::vec3& base_b,
    float contrast) {
	std::vector<unsigned char> pixels(width * height * 4);
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			const float large = tile_noise(x / 3, y / 3, 13);
			const float fine = tile_noise(x, y, 31);
			const float mix_value = glm::clamp(large * 0.75f + fine * 0.25f, 0.0f, 1.0f);
			glm::vec3 color = glm::mix(base_a, base_b, glm::pow(mix_value, contrast));
			color *= glm::mix(0.9f, 1.1f, fine);

			const size_t index = static_cast<size_t>(y * width + x) * 4;
			pixels[index + 0] = to_byte(color.r);
			pixels[index + 1] = to_byte(color.g);
			pixels[index + 2] = to_byte(color.b);
			pixels[index + 3] = 255;
		}
	}

	return pixels;
}

std::vector<unsigned char> build_normal_pixels(int width, int height, float intensity, int seed) {
	std::vector<unsigned char> pixels(width * height * 4);
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			const float nx = tile_noise(x + 1, y, seed) - tile_noise(x - 1, y, seed);
			const float ny = tile_noise(x, y + 1, seed) - tile_noise(x, y - 1, seed);
			glm::vec3 normal = glm::normalize(glm::vec3(-nx * intensity, -ny * intensity, 1.0f));

			const size_t index = static_cast<size_t>(y * width + x) * 4;
			pixels[index + 0] = to_byte(normal.x * 0.5f + 0.5f);
			pixels[index + 1] = to_byte(normal.y * 0.5f + 0.5f);
			pixels[index + 2] = to_byte(normal.z * 0.5f + 0.5f);
			pixels[index + 3] = 255;
		}
	}

	return pixels;
}

std::vector<unsigned char> build_packed_pixels(int width, int height, float height_base, float roughness_base, int seed) {
	std::vector<unsigned char> pixels(width * height * 4);
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			const float large = tile_noise(x / 2, y / 2, seed);
			const float fine = tile_noise(x, y, seed + 29);
			const float packed_height = glm::clamp(height_base * 0.7f + large * 0.3f, 0.0f, 1.0f);
			const float roughness = glm::clamp(roughness_base * 0.8f + fine * 0.2f, 0.0f, 1.0f);

			const size_t index = static_cast<size_t>(y * width + x) * 4;
			pixels[index + 0] = to_byte(packed_height);
			pixels[index + 1] = to_byte(roughness);
			pixels[index + 2] = 0;
			pixels[index + 3] = 255;
		}
	}

	return pixels;
}

}

void DefaultAssetCache::create_all_defaults() {
	get_texture();
	get_shader();
	get_color_shader();
	get_material();
	get_color_material();
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

AssetHandle DefaultAssetCache::get_terrain_shader() {
	if (m_terrain_shader.has_value())
		return *m_terrain_shader;

	m_terrain_shader = AssetFactory::get_instance().get_instance().create_synthetic_asset<Shader>(
	    "Terrain Shader",
	    UUID::from_hash("ENGINE_TERRAIN_SHADER"));

	auto shader = AssetManager::get_instance().get_asset<Shader>(*m_terrain_shader);
	shader->set_source(make_scope<ShaderSourceFile>(
	    PathContextService::get_instance().editor().fallback_root() / "shaders" / "terrain_shader.glsl"));
	shader->resolve();
	return *m_terrain_shader;
}

AssetHandle DefaultAssetCache::get_water_shader() {
	if (m_water_shader.has_value())
		return *m_water_shader;

	m_water_shader = AssetFactory::get_instance().get_instance().create_synthetic_asset<Shader>(
	    "Water Shader",
	    UUID::from_hash("ENGINE_WATER_SHADER"));

	auto shader = AssetManager::get_instance().get_asset<Shader>(*m_water_shader);
	shader->set_source(make_scope<ShaderSourceFile>(
	    PathContextService::get_instance().editor().fallback_root() / "shaders" / "water_shader.glsl"));
	shader->resolve();
	return *m_water_shader;
}

AssetHandle DefaultAssetCache::get_color_shader() {
	if (m_color_shader.has_value())
		return *m_color_shader;

	m_color_shader = AssetFactory::get_instance().get_instance().create_synthetic_asset<Shader>(
	    "Default Color Shader",
	    UUID::from_hash("ENGINE_DEFAULT_COLOR_SHADER"));

	auto shader = AssetManager::get_instance().get_asset<Shader>(*m_color_shader);
	shader->set_source(make_scope<ShaderSourceFile>(
	    PathContextService::get_instance().editor().fallback_root() / "shaders" / "color_shader.glsl"));
	shader->resolve();

	return *m_color_shader;
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

AssetHandle DefaultAssetCache::get_terrain_ground_albedo() {
	if (!m_terrain_ground_albedo) {
		m_terrain_ground_albedo = create_runtime_texture(
		    "Terrain Ground Albedo",
		    UUID::from_hash("ENGINE_TERRAIN_GROUND_ALBEDO"),
		    64,
		    64,
		    build_albedo_pixels(64, 64, { 0.16f, 0.33f, 0.12f }, { 0.38f, 0.36f, 0.18f }, 0.85f),
		    TextureWrapMode::Repeat);
	}
	return *m_terrain_ground_albedo;
}

AssetHandle DefaultAssetCache::get_terrain_ground_normal() {
	if (!m_terrain_ground_normal) {
		m_terrain_ground_normal = create_runtime_texture(
		    "Terrain Ground Normal",
		    UUID::from_hash("ENGINE_TERRAIN_GROUND_NORMAL"),
		    64,
		    64,
		    build_normal_pixels(64, 64, 0.45f, 101),
		    TextureWrapMode::Repeat);
	}
	return *m_terrain_ground_normal;
}

AssetHandle DefaultAssetCache::get_terrain_ground_packed() {
	if (!m_terrain_ground_packed) {
		m_terrain_ground_packed = create_runtime_texture(
		    "Terrain Ground Packed",
		    UUID::from_hash("ENGINE_TERRAIN_GROUND_PACKED"),
		    64,
		    64,
		    build_packed_pixels(64, 64, 0.45f, 0.84f, 131),
		    TextureWrapMode::Repeat);
	}
	return *m_terrain_ground_packed;
}

AssetHandle DefaultAssetCache::get_terrain_rock_albedo() {
	if (!m_terrain_rock_albedo) {
		m_terrain_rock_albedo = create_runtime_texture(
		    "Terrain Rock Albedo",
		    UUID::from_hash("ENGINE_TERRAIN_ROCK_ALBEDO"),
		    64,
		    64,
		    build_albedo_pixels(64, 64, { 0.30f, 0.31f, 0.33f }, { 0.50f, 0.47f, 0.45f }, 1.25f),
		    TextureWrapMode::Repeat);
	}
	return *m_terrain_rock_albedo;
}

AssetHandle DefaultAssetCache::get_terrain_rock_normal() {
	if (!m_terrain_rock_normal) {
		m_terrain_rock_normal = create_runtime_texture(
		    "Terrain Rock Normal",
		    UUID::from_hash("ENGINE_TERRAIN_ROCK_NORMAL"),
		    64,
		    64,
		    build_normal_pixels(64, 64, 0.95f, 211),
		    TextureWrapMode::Repeat);
	}
	return *m_terrain_rock_normal;
}

AssetHandle DefaultAssetCache::get_terrain_rock_packed() {
	if (!m_terrain_rock_packed) {
		m_terrain_rock_packed = create_runtime_texture(
		    "Terrain Rock Packed",
		    UUID::from_hash("ENGINE_TERRAIN_ROCK_PACKED"),
		    64,
		    64,
		    build_packed_pixels(64, 64, 0.76f, 0.58f, 223),
		    TextureWrapMode::Repeat);
	}
	return *m_terrain_rock_packed;
}

AssetHandle DefaultAssetCache::get_terrain_sand_albedo() {
	if (!m_terrain_sand_albedo) {
		m_terrain_sand_albedo = create_runtime_texture(
		    "Terrain Sand Albedo",
		    UUID::from_hash("ENGINE_TERRAIN_SAND_ALBEDO"),
		    64,
		    64,
		    build_albedo_pixels(64, 64, { 0.64f, 0.58f, 0.44f }, { 0.80f, 0.76f, 0.61f }, 0.95f),
		    TextureWrapMode::Repeat);
	}
	return *m_terrain_sand_albedo;
}

AssetHandle DefaultAssetCache::get_terrain_sand_normal() {
	if (!m_terrain_sand_normal) {
		m_terrain_sand_normal = create_runtime_texture(
		    "Terrain Sand Normal",
		    UUID::from_hash("ENGINE_TERRAIN_SAND_NORMAL"),
		    64,
		    64,
		    build_normal_pixels(64, 64, 0.22f, 307),
		    TextureWrapMode::Repeat);
	}
	return *m_terrain_sand_normal;
}

AssetHandle DefaultAssetCache::get_terrain_sand_packed() {
	if (!m_terrain_sand_packed) {
		m_terrain_sand_packed = create_runtime_texture(
		    "Terrain Sand Packed",
		    UUID::from_hash("ENGINE_TERRAIN_SAND_PACKED"),
		    64,
		    64,
		    build_packed_pixels(64, 64, 0.24f, 0.92f, 331),
		    TextureWrapMode::Repeat);
	}
	return *m_terrain_sand_packed;
}

AssetHandle DefaultAssetCache::get_terrain_snow_albedo() {
	if (!m_terrain_snow_albedo) {
		m_terrain_snow_albedo = create_runtime_texture(
		    "Terrain Snow Albedo",
		    UUID::from_hash("ENGINE_TERRAIN_SNOW_ALBEDO"),
		    64,
		    64,
		    build_albedo_pixels(64, 64, { 0.88f, 0.90f, 0.94f }, { 0.98f, 0.99f, 1.00f }, 0.55f),
		    TextureWrapMode::Repeat);
	}
	return *m_terrain_snow_albedo;
}

AssetHandle DefaultAssetCache::get_terrain_snow_normal() {
	if (!m_terrain_snow_normal) {
		m_terrain_snow_normal = create_runtime_texture(
		    "Terrain Snow Normal",
		    UUID::from_hash("ENGINE_TERRAIN_SNOW_NORMAL"),
		    64,
		    64,
		    build_normal_pixels(64, 64, 0.12f, 401),
		    TextureWrapMode::Repeat);
	}
	return *m_terrain_snow_normal;
}

AssetHandle DefaultAssetCache::get_terrain_snow_packed() {
	if (!m_terrain_snow_packed) {
		m_terrain_snow_packed = create_runtime_texture(
		    "Terrain Snow Packed",
		    UUID::from_hash("ENGINE_TERRAIN_SNOW_PACKED"),
		    64,
		    64,
		    build_packed_pixels(64, 64, 0.18f, 0.36f, 419),
		    TextureWrapMode::Repeat);
	}
	return *m_terrain_snow_packed;
}

AssetHandle DefaultAssetCache::get_terrain_micro_normal() {
	if (!m_terrain_micro_normal) {
		m_terrain_micro_normal = create_runtime_texture(
		    "Terrain Micro Normal",
		    UUID::from_hash("ENGINE_TERRAIN_MICRO_NORMAL"),
		    64,
		    64,
		    build_normal_pixels(64, 64, 0.60f, 503),
		    TextureWrapMode::Repeat);
	}
	return *m_terrain_micro_normal;
}

AssetHandle DefaultAssetCache::get_water_wave_normal_a() {
	if (!m_water_wave_normal_a) {
		m_water_wave_normal_a = create_runtime_texture(
		    "Water Wave Normal A",
		    UUID::from_hash("ENGINE_WATER_WAVE_A"),
		    64,
		    64,
		    build_normal_pixels(64, 64, 0.35f, 601),
		    TextureWrapMode::Repeat);
	}
	return *m_water_wave_normal_a;
}

AssetHandle DefaultAssetCache::get_water_wave_normal_b() {
	if (!m_water_wave_normal_b) {
		m_water_wave_normal_b = create_runtime_texture(
		    "Water Wave Normal B",
		    UUID::from_hash("ENGINE_WATER_WAVE_B"),
		    64,
		    64,
		    build_normal_pixels(64, 64, 0.48f, 647),
		    TextureWrapMode::Repeat);
	}
	return *m_water_wave_normal_b;
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

AssetHandle DefaultAssetCache::get_color_material() {
	if (m_color_material.has_value())
		return *m_color_material;

	m_color_material = AssetFactory::get_instance().get_instance().create_synthetic_asset<MaterialColor>(
	    "Default Color Material",
	    UUID::from_arbitrary_string("DEFAULT_COLOR_MATERIAL_2D"));

	auto material = AssetManager::get_instance().get_asset<MaterialColor>(*m_color_material);
	material->set_shader(get_color_shader());
	material->set_unlit(false);
	material->resolve();

	return *m_color_material;
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
