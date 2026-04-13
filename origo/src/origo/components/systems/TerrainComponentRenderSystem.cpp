#include "TerrainComponentRenderSystem.h"

#include "origo/assets/AssetManager.h"
#include "origo/assets/DefaultAssetCache.h"
#include "origo/assets/AssetFactory.h"
#include "origo/assets/material/TerrainMaterial.h"
#include "origo/assets/material/WaterMaterial.h"

#include "origo/components/TerrainComponent.h"
#include "origo/components/TransformComponent.h"

#include "origo/scene/SystemScheduler.h"

namespace Origo {
namespace {
OptionalAssetHandle pick_texture_handle(const OptionalAssetHandle& preferred, AssetHandle fallback) {
	return preferred ? preferred : OptionalAssetHandle { fallback };
}
}

static TerrainMaterial* ensure_terrain_material(TerrainComponent& terrain_component, int entity_id) {
	auto& asset_manager = AssetManager::get_instance();
	if (terrain_component.terrain_material
	    && asset_manager.is_valid(*terrain_component.terrain_material)) {
		return asset_manager.get_asset<TerrainMaterial>(*terrain_component.terrain_material);
	}

	terrain_component.terrain_material = AssetFactory::get_instance().create_runtime_asset<TerrainMaterial>(
	    "TerrainMaterial_" + std::to_string(entity_id));
	auto* material = asset_manager.get_asset<TerrainMaterial>(*terrain_component.terrain_material);
	if (!material)
		return nullptr;

	material->set_shader(DefaultAssetCache::get_instance().get_terrain_shader());
	material->resolve();
	return material;
}

static WaterMaterial* ensure_water_material(TerrainComponent& terrain_component, int entity_id) {
	auto& asset_manager = AssetManager::get_instance();
	if (terrain_component.water_material
	    && asset_manager.is_valid(*terrain_component.water_material)) {
		return asset_manager.get_asset<WaterMaterial>(*terrain_component.water_material);
	}

	terrain_component.water_material = AssetFactory::get_instance().create_runtime_asset<WaterMaterial>(
	    "WaterMaterial_" + std::to_string(entity_id));
	auto* material = asset_manager.get_asset<WaterMaterial>(*terrain_component.water_material);
	if (!material)
		return nullptr;

	material->set_shader(DefaultAssetCache::get_instance().get_water_shader());
	material->set_wave_a(DefaultAssetCache::get_instance().get_water_wave_normal_a());
	material->set_wave_b(DefaultAssetCache::get_instance().get_water_wave_normal_b());
	material->resolve();
	return material;
}

void TerrainComponentRenderSystem::render(Scene* scene, RenderContext& context) {
	scene->view<TerrainComponent, TransformComponent>(
	    [&](RID entity,
	        TerrainComponent& terrain_component,
	        TransformComponent& transform) {
		    if (terrain_component.should_rebuild || terrain_component.started_rebuilding)
			    return;

		    if (!terrain_component.terrain_mesh.has_value())
			    return;

		    if (!AssetManager::get_instance().is_valid(*terrain_component.terrain_mesh))
			    return;

		    auto* terrain_material = ensure_terrain_material(terrain_component, entity.get_id());
		    if (!terrain_material)
			    return;

		    terrain_material->set_ground_albedo(pick_texture_handle(terrain_component.ground_albedo, DefaultAssetCache::get_instance().get_terrain_ground_albedo()))
		        .set_ground_normal(pick_texture_handle(terrain_component.ground_normal, DefaultAssetCache::get_instance().get_terrain_ground_normal()))
		        .set_ground_packed(pick_texture_handle(terrain_component.ground_packed, DefaultAssetCache::get_instance().get_terrain_ground_packed()))
		        .set_rock_albedo(pick_texture_handle(terrain_component.rock_albedo, DefaultAssetCache::get_instance().get_terrain_rock_albedo()))
		        .set_rock_normal(pick_texture_handle(terrain_component.rock_normal, DefaultAssetCache::get_instance().get_terrain_rock_normal()))
		        .set_rock_packed(pick_texture_handle(terrain_component.rock_packed, DefaultAssetCache::get_instance().get_terrain_rock_packed()))
		        .set_sand_albedo(pick_texture_handle(terrain_component.sand_albedo, DefaultAssetCache::get_instance().get_terrain_sand_albedo()))
		        .set_sand_normal(pick_texture_handle(terrain_component.sand_normal, DefaultAssetCache::get_instance().get_terrain_sand_normal()))
		        .set_sand_packed(pick_texture_handle(terrain_component.sand_packed, DefaultAssetCache::get_instance().get_terrain_sand_packed()))
		        .set_snow_albedo(pick_texture_handle(terrain_component.snow_albedo, DefaultAssetCache::get_instance().get_terrain_snow_albedo()))
		        .set_snow_normal(pick_texture_handle(terrain_component.snow_normal, DefaultAssetCache::get_instance().get_terrain_snow_normal()))
		        .set_snow_packed(pick_texture_handle(terrain_component.snow_packed, DefaultAssetCache::get_instance().get_terrain_snow_packed()))
		        .set_micro_normal(pick_texture_handle(terrain_component.micro_normal, DefaultAssetCache::get_instance().get_terrain_micro_normal()));

		    terrain_material->set_uniform("u_use_texture_layers", terrain_component.use_texture_layers)
		        .set_uniform("u_terrain_max_height", terrain_component.height)
		        .set_uniform("u_water_level", terrain_component.water_settings.water_level)
		        .set_uniform("u_ground_tile_scale", terrain_component.surface_settings.ground_tile_scale)
		        .set_uniform("u_rock_tile_scale", terrain_component.surface_settings.rock_tile_scale)
		        .set_uniform("u_sand_tile_scale", terrain_component.surface_settings.sand_tile_scale)
		        .set_uniform("u_snow_tile_scale", terrain_component.surface_settings.snow_tile_scale)
		        .set_uniform("u_micro_detail_scale", terrain_component.surface_settings.micro_detail_scale)
		        .set_uniform("u_micro_detail_strength", terrain_component.surface_settings.micro_detail_strength)
		        .set_uniform("u_rock_slope_start", terrain_component.surface_settings.rock_slope_start)
		        .set_uniform("u_rock_slope_end", terrain_component.surface_settings.rock_slope_end)
		        .set_uniform("u_cliff_slope_start", terrain_component.surface_settings.cliff_slope_start)
		        .set_uniform("u_cliff_slope_end", terrain_component.surface_settings.cliff_slope_end)
		        .set_uniform("u_shoreline_width", terrain_component.surface_settings.shoreline_width)
		        .set_uniform("u_wetness_width", terrain_component.surface_settings.wetness_width)
		        .set_uniform("u_snow_start", terrain_component.surface_settings.snow_start)
		        .set_uniform("u_snow_end", terrain_component.surface_settings.snow_end)
		        .set_uniform("u_snow_slope_limit", terrain_component.surface_settings.snow_slope_limit)
		        .set_uniform("u_height_blend_sharpness", terrain_component.surface_settings.height_blend_sharpness);

		    context.submit_mesh(
		        *terrain_component.terrain_mesh,
		        *terrain_component.terrain_material,
		        transform.get_model_matrix(),
		        RenderPass::Geometry,
		        GL_TRIANGLES,
		        entity.get_id());

		    if (!terrain_component.water_settings.enabled || !terrain_component.water_mesh
		        || !AssetManager::get_instance().is_valid(*terrain_component.water_mesh)) {
			    return;
		    }

		    auto* water_material = ensure_water_material(terrain_component, entity.get_id());
		    if (!water_material)
			    return;

		    water_material
		        ->set_uniform("u_wave_tile_scale", terrain_component.water_settings.wave_tile_scale)
		        .set_uniform("u_wave_strength", terrain_component.water_settings.wave_strength)
		        .set_uniform("u_smoothness", terrain_component.water_settings.smoothness);

		    context.submit_mesh(
		        *terrain_component.water_mesh,
		        *terrain_component.water_material,
		        transform.get_model_matrix(),
		        RenderPass::Geometry,
		        GL_TRIANGLES,
		        entity.get_id());
	    });
}

}

REGISTER_RENDER_SYSTEM(
    Origo::GamePhase::RenderAlways,
    Origo::TerrainComponentRenderSystem)
