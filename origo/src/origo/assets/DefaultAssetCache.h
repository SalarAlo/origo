#pragma once

#include "AssetManager.h"

#include "origo/utils/Singleton.h"

namespace Origo {

class DefaultAssetCache : public Singleton<DefaultAssetCache> {
public:
	void create_all_defaults();

	AssetHandle get_shader();
	AssetHandle get_terrain_shader();
	AssetHandle get_water_shader();
	AssetHandle get_color_shader();
	AssetHandle get_texture();
	AssetHandle get_terrain_ground_albedo();
	AssetHandle get_terrain_ground_normal();
	AssetHandle get_terrain_ground_packed();
	AssetHandle get_terrain_rock_albedo();
	AssetHandle get_terrain_rock_normal();
	AssetHandle get_terrain_rock_packed();
	AssetHandle get_terrain_sand_albedo();
	AssetHandle get_terrain_sand_normal();
	AssetHandle get_terrain_sand_packed();
	AssetHandle get_terrain_snow_albedo();
	AssetHandle get_terrain_snow_normal();
	AssetHandle get_terrain_snow_packed();
	AssetHandle get_terrain_micro_normal();
	AssetHandle get_water_wave_normal_a();
	AssetHandle get_water_wave_normal_b();

	AssetHandle get_material();
	AssetHandle get_color_material();
	AssetHandle get_outline_material();
	AssetHandle get_debug_material();
	AssetHandle get_particle_material();

private:
	std::optional<AssetHandle> m_shader;
	std::optional<AssetHandle> m_terrain_shader;
	std::optional<AssetHandle> m_water_shader;
	std::optional<AssetHandle> m_color_shader;
	std::optional<AssetHandle> m_texture;
	std::optional<AssetHandle> m_terrain_ground_albedo;
	std::optional<AssetHandle> m_terrain_ground_normal;
	std::optional<AssetHandle> m_terrain_ground_packed;
	std::optional<AssetHandle> m_terrain_rock_albedo;
	std::optional<AssetHandle> m_terrain_rock_normal;
	std::optional<AssetHandle> m_terrain_rock_packed;
	std::optional<AssetHandle> m_terrain_sand_albedo;
	std::optional<AssetHandle> m_terrain_sand_normal;
	std::optional<AssetHandle> m_terrain_sand_packed;
	std::optional<AssetHandle> m_terrain_snow_albedo;
	std::optional<AssetHandle> m_terrain_snow_normal;
	std::optional<AssetHandle> m_terrain_snow_packed;
	std::optional<AssetHandle> m_terrain_micro_normal;
	std::optional<AssetHandle> m_water_wave_normal_a;
	std::optional<AssetHandle> m_water_wave_normal_b;
	std::optional<AssetHandle> m_material;
	std::optional<AssetHandle> m_color_material;
	std::optional<AssetHandle> m_outline_material;
	std::optional<AssetHandle> m_particle_emission_debug_material;
	std::optional<AssetHandle> m_particle_material;
};

}
