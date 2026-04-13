#include <algorithm>

#include "imgui.h"

#include "origo/components/TerrainComponent.h"

#include "ui/ComponentUI.h"
#include "ui/InspectorDrawRegistry.h"

namespace OrigoEditor {
namespace {

void apply_style_controls(
    Origo::TerrainComponent& terrain,
    float landform,
    float detail,
    float erosion,
    float snow,
    float shoreline) {
	terrain.shape_settings.base_strength = 0.56f + landform * 0.20f;
	terrain.shape_settings.ridge_strength = 0.14f + landform * 0.34f;
	terrain.shape_settings.medium_detail_strength = 0.08f + detail * 0.22f;
	terrain.shape_settings.detail_strength = 0.02f + detail * 0.12f;
	terrain.shape_settings.ridge_frequency_multiplier = 1.15f + landform * 0.95f;
	terrain.shape_settings.fine_frequency_multiplier = 2.2f + detail * 2.4f;
	terrain.shape_settings.domain_warp_strength = 8.0f + landform * 18.0f;
	terrain.contrast = 0.88f + landform * 0.55f;

	terrain.erosion_settings.droplet_count = static_cast<int>(15000.0f + erosion * 105000.0f);
	terrain.erosion_settings.max_lifetime = static_cast<int>(18.0f + erosion * 34.0f);
	terrain.erosion_settings.erode_speed = 0.12f + erosion * 0.36f;
	terrain.erosion_settings.deposit_speed = 0.10f + erosion * 0.34f;
	terrain.erosion_settings.sediment_capacity_factor = 2.0f + erosion * 6.0f;
	terrain.erosion_settings.gravity = 2.2f + erosion * 4.2f;

	terrain.surface_settings.micro_detail_strength = 0.08f + detail * 0.44f;
	terrain.surface_settings.micro_detail_scale = 0.14f + detail * 0.22f;

	terrain.surface_settings.snow_start = 0.90f - snow * 0.24f;
	terrain.surface_settings.snow_end = glm::clamp(terrain.surface_settings.snow_start + 0.10f, 0.0f, 1.0f);
	terrain.surface_settings.snow_slope_limit = 0.14f + snow * 0.22f;

	terrain.surface_settings.shoreline_width = 0.015f + shoreline * 0.090f;
	terrain.surface_settings.wetness_width = 0.010f + shoreline * 0.070f;
	terrain.surface_settings.rock_slope_start = 0.12f + landform * 0.14f;
	terrain.surface_settings.rock_slope_end = 0.34f + landform * 0.22f;
	terrain.surface_settings.cliff_slope_start = 0.24f + landform * 0.16f;
	terrain.surface_settings.cliff_slope_end = 0.52f + landform * 0.18f;
}

}

static bool s_registered = []() {
	InspectorDrawRegistry::get_instance().register_native_drawer<Origo::TerrainComponent>(
	    "Terrain",
	    "./icons/Mesh.svg",
	    [](Origo::TerrainComponent& terrain) {
		    const auto before_noise_settings = terrain.noise_settings;
		    const auto before_shape_settings = terrain.shape_settings;
		    const auto before_surface_settings = terrain.surface_settings;
		    const auto before_water_settings = terrain.water_settings;
		    const auto before_erosion_settings = terrain.erosion_settings;
		    const bool before_use_texture_layers = terrain.use_texture_layers;
		    const auto before_ground_albedo = terrain.ground_albedo;
		    const auto before_ground_normal = terrain.ground_normal;
		    const auto before_ground_packed = terrain.ground_packed;
		    const auto before_rock_albedo = terrain.rock_albedo;
		    const auto before_rock_normal = terrain.rock_normal;
		    const auto before_rock_packed = terrain.rock_packed;
		    const auto before_sand_albedo = terrain.sand_albedo;
		    const auto before_sand_normal = terrain.sand_normal;
		    const auto before_sand_packed = terrain.sand_packed;
		    const auto before_snow_albedo = terrain.snow_albedo;
		    const auto before_snow_normal = terrain.snow_normal;
		    const auto before_snow_packed = terrain.snow_packed;
		    const auto before_micro_normal = terrain.micro_normal;
		    const int before_size_x = terrain.size_x;
		    const int before_size_z = terrain.size_z;
		    const float before_scale = terrain.scale;
		    const float before_cell_size = terrain.cell_size;
		    const float before_height = terrain.height;
		    const float before_contrast = terrain.contrast;

		    if (terrain.started_rebuilding)
			    ImGui::TextDisabled("Rebuilding...");

		    ImGui::BeginDisabled(terrain.should_rebuild);

		    const bool general_open = ComponentUI::start_region("Terrain");
		    if (general_open) {
			    ComponentUI::draw_int_control("Size X", terrain.size_x);
			    ComponentUI::draw_int_control("Size Z", terrain.size_z);
			    ComponentUI::draw_float_control("Noise Scale", terrain.scale);
			    ComponentUI::draw_float_control("Cell Size", terrain.cell_size);
			    ComponentUI::draw_float_control("Height", terrain.height);

			    terrain.size_x = std::max(1, terrain.size_x);
			    terrain.size_z = std::max(1, terrain.size_z);
			    terrain.scale = std::max(0.001f, terrain.scale);
			    terrain.cell_size = std::max(0.05f, terrain.cell_size);
			    terrain.height = std::max(0.0f, terrain.height);
		    }
		    ComponentUI::end_region(general_open);

		    const bool look_open = ComponentUI::start_region("Look");
		    if (look_open) {
			    float landform = glm::clamp((terrain.shape_settings.ridge_strength - 0.14f) / 0.34f, 0.0f, 1.0f);
			    float detail = glm::clamp((terrain.shape_settings.detail_strength - 0.02f) / 0.12f, 0.0f, 1.0f);
			    float erosion = glm::clamp((static_cast<float>(terrain.erosion_settings.droplet_count) - 15000.0f) / 105000.0f, 0.0f, 1.0f);
			    float snow = glm::clamp((0.90f - terrain.surface_settings.snow_start) / 0.24f, 0.0f, 1.0f);
			    float shoreline = glm::clamp((terrain.surface_settings.shoreline_width - 0.015f) / 0.090f, 0.0f, 1.0f);

			    ComponentUI::draw_float_slider_control("Landform", landform, 0.0f, 1.0f);
			    ComponentUI::draw_float_slider_control("Detail", detail, 0.0f, 1.0f);
			    ComponentUI::draw_float_slider_control("Erosion", erosion, 0.0f, 1.0f);
			    ComponentUI::draw_float_slider_control("Snow", snow, 0.0f, 1.0f);
			    ComponentUI::draw_float_slider_control("Shoreline", shoreline, 0.0f, 1.0f);
			    apply_style_controls(terrain, landform, detail, erosion, snow, shoreline);
		    }
		    ComponentUI::end_region(look_open);

		    const bool textures_open = ComponentUI::start_region("Textures");
		    if (textures_open) {
			    ComponentUI::draw_bool_control("Use Layer Textures", terrain.use_texture_layers);
			    if (terrain.use_texture_layers) {
				    ComponentUI::draw_asset_control("Ground Albedo", terrain.ground_albedo, Origo::AssetType::Texture2D);
				    ComponentUI::draw_asset_control("Ground Normal", terrain.ground_normal, Origo::AssetType::Texture2D);
				    ComponentUI::draw_asset_control("Ground Packed", terrain.ground_packed, Origo::AssetType::Texture2D);
				    ComponentUI::draw_asset_control("Rock Albedo", terrain.rock_albedo, Origo::AssetType::Texture2D);
				    ComponentUI::draw_asset_control("Rock Normal", terrain.rock_normal, Origo::AssetType::Texture2D);
				    ComponentUI::draw_asset_control("Rock Packed", terrain.rock_packed, Origo::AssetType::Texture2D);
				    ComponentUI::draw_asset_control("Sand Albedo", terrain.sand_albedo, Origo::AssetType::Texture2D);
				    ComponentUI::draw_asset_control("Sand Normal", terrain.sand_normal, Origo::AssetType::Texture2D);
				    ComponentUI::draw_asset_control("Sand Packed", terrain.sand_packed, Origo::AssetType::Texture2D);
				    ComponentUI::draw_asset_control("Snow Albedo", terrain.snow_albedo, Origo::AssetType::Texture2D);
				    ComponentUI::draw_asset_control("Snow Normal", terrain.snow_normal, Origo::AssetType::Texture2D);
				    ComponentUI::draw_asset_control("Snow Packed", terrain.snow_packed, Origo::AssetType::Texture2D);
				    ComponentUI::draw_asset_control("Micro Normal", terrain.micro_normal, Origo::AssetType::Texture2D);
			    } else {
				    ImGui::TextDisabled("Using procedural terrain colors by default.");
			    }
		    }
		    ComponentUI::end_region(textures_open);

		    const bool water_open = ComponentUI::start_region("Water");
		    if (water_open) {
			    ComponentUI::draw_bool_control("Enabled", terrain.water_settings.enabled);
			    ComponentUI::draw_float_control("Water Level", terrain.water_settings.water_level, 0.01f, "%.3f");
			    ComponentUI::draw_float_control("Wave Tile", terrain.water_settings.wave_tile_scale, 0.001f, "%.3f");
			    ComponentUI::draw_float_control("Wave Strength", terrain.water_settings.wave_strength, 0.01f, "%.3f");
			    ComponentUI::draw_float_control("Smoothness", terrain.water_settings.smoothness, 0.01f, "%.3f");
		    }
		    ComponentUI::end_region(water_open);

		    const bool advanced_open = ComponentUI::start_region("Advanced", false);
		    if (advanced_open) {
			    ComponentUI::draw_enum_control("Noise Type", terrain.noise_settings.type);
			    ComponentUI::draw_int_control("Seed", terrain.noise_settings.seed);
			    ComponentUI::draw_float_control("Frequency", terrain.noise_settings.frequency, 0.001f, "%.4f");
			    ComponentUI::draw_enum_control("Fractal", terrain.noise_settings.fractal);
			    ComponentUI::draw_int_control("Octaves", terrain.noise_settings.octaves);
			    ComponentUI::draw_float_control("Lacunarity", terrain.noise_settings.lacunarity, 0.01f, "%.3f");
			    ComponentUI::draw_float_control("Gain", terrain.noise_settings.gain, 0.01f, "%.3f");
			    ComponentUI::draw_float_control("Weighted Strength", terrain.noise_settings.weighted_strength, 0.01f, "%.3f");

			    ComponentUI::draw_float_control("Base Freq", terrain.shape_settings.base_frequency_multiplier, 0.01f, "%.3f");
			    ComponentUI::draw_float_control("Ridge Freq", terrain.shape_settings.ridge_frequency_multiplier, 0.01f, "%.3f");
			    ComponentUI::draw_float_control("Medium Freq", terrain.shape_settings.medium_frequency_multiplier, 0.01f, "%.3f");
			    ComponentUI::draw_float_control("Fine Freq", terrain.shape_settings.fine_frequency_multiplier, 0.01f, "%.3f");
			    ComponentUI::draw_float_control("Base Strength", terrain.shape_settings.base_strength, 0.01f, "%.3f");
			    ComponentUI::draw_float_control("Ridge Strength", terrain.shape_settings.ridge_strength, 0.01f, "%.3f");
			    ComponentUI::draw_float_control("Medium Detail", terrain.shape_settings.medium_detail_strength, 0.01f, "%.3f");
			    ComponentUI::draw_float_control("Detail Strength", terrain.shape_settings.detail_strength, 0.01f, "%.3f");
			    ComponentUI::draw_float_control("Warp Strength", terrain.shape_settings.domain_warp_strength, 0.05f, "%.3f");
			    ComponentUI::draw_float_control("Remap Min", terrain.shape_settings.remap_min, 0.01f, "%.3f");
			    ComponentUI::draw_float_control("Remap Max", terrain.shape_settings.remap_max, 0.01f, "%.3f");

			    ComponentUI::draw_int_control("Droplet Count", terrain.erosion_settings.droplet_count);
			    ComponentUI::draw_int_control("Max Lifetime", terrain.erosion_settings.max_lifetime);
			    ComponentUI::draw_float_control("Capacity", terrain.erosion_settings.sediment_capacity_factor, 0.01f, "%.3f");
			    ComponentUI::draw_float_control("Min Capacity", terrain.erosion_settings.min_sediment_capacity, 0.001f, "%.3f");
			    ComponentUI::draw_float_control("Erode Speed", terrain.erosion_settings.erode_speed, 0.01f, "%.3f");
			    ComponentUI::draw_float_control("Deposit Speed", terrain.erosion_settings.deposit_speed, 0.01f, "%.3f");
			    ComponentUI::draw_float_control("Evaporation", terrain.erosion_settings.evaporate_speed, 0.001f, "%.3f");
			    ComponentUI::draw_float_control("Gravity", terrain.erosion_settings.gravity, 0.01f, "%.3f");
			    ComponentUI::draw_float_control("Inertia", terrain.erosion_settings.inertia, 0.01f, "%.3f");
		    }
		    ComponentUI::end_region(advanced_open);

		    const bool changed = terrain.noise_settings.type != before_noise_settings.type
		        || terrain.noise_settings.seed != before_noise_settings.seed
		        || terrain.noise_settings.frequency != before_noise_settings.frequency
		        || terrain.noise_settings.fractal != before_noise_settings.fractal
		        || terrain.noise_settings.octaves != before_noise_settings.octaves
		        || terrain.noise_settings.lacunarity != before_noise_settings.lacunarity
		        || terrain.noise_settings.gain != before_noise_settings.gain
		        || terrain.noise_settings.weighted_strength != before_noise_settings.weighted_strength
		        || terrain.shape_settings.base_frequency_multiplier != before_shape_settings.base_frequency_multiplier
		        || terrain.shape_settings.ridge_frequency_multiplier != before_shape_settings.ridge_frequency_multiplier
		        || terrain.shape_settings.medium_frequency_multiplier != before_shape_settings.medium_frequency_multiplier
		        || terrain.shape_settings.fine_frequency_multiplier != before_shape_settings.fine_frequency_multiplier
		        || terrain.shape_settings.base_strength != before_shape_settings.base_strength
		        || terrain.shape_settings.ridge_strength != before_shape_settings.ridge_strength
		        || terrain.shape_settings.medium_detail_strength != before_shape_settings.medium_detail_strength
		        || terrain.shape_settings.detail_strength != before_shape_settings.detail_strength
		        || terrain.shape_settings.domain_warp_strength != before_shape_settings.domain_warp_strength
		        || terrain.shape_settings.remap_min != before_shape_settings.remap_min
		        || terrain.shape_settings.remap_max != before_shape_settings.remap_max
		        || terrain.erosion_settings.seed != before_erosion_settings.seed
		        || terrain.erosion_settings.droplet_count != before_erosion_settings.droplet_count
		        || terrain.erosion_settings.max_lifetime != before_erosion_settings.max_lifetime
		        || terrain.erosion_settings.sediment_capacity_factor != before_erosion_settings.sediment_capacity_factor
		        || terrain.erosion_settings.min_sediment_capacity != before_erosion_settings.min_sediment_capacity
		        || terrain.erosion_settings.erode_speed != before_erosion_settings.erode_speed
		        || terrain.erosion_settings.deposit_speed != before_erosion_settings.deposit_speed
		        || terrain.erosion_settings.evaporate_speed != before_erosion_settings.evaporate_speed
		        || terrain.erosion_settings.gravity != before_erosion_settings.gravity
		        || terrain.erosion_settings.inertia != before_erosion_settings.inertia
		        || terrain.surface_settings.ground_tile_scale != before_surface_settings.ground_tile_scale
		        || terrain.surface_settings.rock_tile_scale != before_surface_settings.rock_tile_scale
		        || terrain.surface_settings.sand_tile_scale != before_surface_settings.sand_tile_scale
		        || terrain.surface_settings.snow_tile_scale != before_surface_settings.snow_tile_scale
		        || terrain.surface_settings.micro_detail_scale != before_surface_settings.micro_detail_scale
		        || terrain.surface_settings.micro_detail_strength != before_surface_settings.micro_detail_strength
		        || terrain.surface_settings.rock_slope_start != before_surface_settings.rock_slope_start
		        || terrain.surface_settings.rock_slope_end != before_surface_settings.rock_slope_end
		        || terrain.surface_settings.cliff_slope_start != before_surface_settings.cliff_slope_start
		        || terrain.surface_settings.cliff_slope_end != before_surface_settings.cliff_slope_end
		        || terrain.surface_settings.shoreline_width != before_surface_settings.shoreline_width
		        || terrain.surface_settings.wetness_width != before_surface_settings.wetness_width
		        || terrain.surface_settings.snow_start != before_surface_settings.snow_start
		        || terrain.surface_settings.snow_end != before_surface_settings.snow_end
		        || terrain.surface_settings.snow_slope_limit != before_surface_settings.snow_slope_limit
		        || terrain.surface_settings.height_blend_sharpness != before_surface_settings.height_blend_sharpness
		        || terrain.water_settings.enabled != before_water_settings.enabled
		        || terrain.water_settings.water_level != before_water_settings.water_level
		        || terrain.water_settings.wave_tile_scale != before_water_settings.wave_tile_scale
		        || terrain.water_settings.wave_strength != before_water_settings.wave_strength
		        || terrain.water_settings.smoothness != before_water_settings.smoothness
		        || terrain.use_texture_layers != before_use_texture_layers
		        || terrain.ground_albedo != before_ground_albedo
		        || terrain.ground_normal != before_ground_normal
		        || terrain.ground_packed != before_ground_packed
		        || terrain.rock_albedo != before_rock_albedo
		        || terrain.rock_normal != before_rock_normal
		        || terrain.rock_packed != before_rock_packed
		        || terrain.sand_albedo != before_sand_albedo
		        || terrain.sand_normal != before_sand_normal
		        || terrain.sand_packed != before_sand_packed
		        || terrain.snow_albedo != before_snow_albedo
		        || terrain.snow_normal != before_snow_normal
		        || terrain.snow_packed != before_snow_packed
		        || terrain.micro_normal != before_micro_normal
		        || terrain.size_x != before_size_x
		        || terrain.size_z != before_size_z
		        || terrain.scale != before_scale
		        || terrain.cell_size != before_cell_size
		        || terrain.height != before_height
		        || terrain.contrast != before_contrast;

		    ImGui::EndDisabled();

		    if (changed)
			    terrain.should_rebuild = true;
	    });

	return true;
}();

}
