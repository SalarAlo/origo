#include <algorithm>

#include "imgui.h"

#include "origo/components/TerrainComponent.h"

#include "ui/ComponentUI.h"
#include "ui/InspectorDrawRegistry.h"

namespace OrigoEditor {

static bool s_registered = []() {
	InspectorDrawRegistry::get_instance().register_native_drawer<Origo::TerrainComponent>(
	    "Terrain",
	    "./icons/Mesh.svg",
	    [](Origo::TerrainComponent& terrain) {
		    const auto before_noise_settings = terrain.noise_settings;
		    const auto before_erosion_settings = terrain.erosion_settings;
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
			    ComponentUI::draw_float_control("Scale", terrain.scale);
			    ComponentUI::draw_float_control("Cell Size", terrain.cell_size);
			    ComponentUI::draw_float_control("Height", terrain.height);
			    ComponentUI::draw_float_control("Contrast", terrain.contrast);

			    terrain.size_x = std::max(1, terrain.size_x);
			    terrain.size_z = std::max(1, terrain.size_z);
			    terrain.scale = std::max(0.001f, terrain.scale);
			    terrain.cell_size = std::max(0.05f, terrain.cell_size);
			    terrain.height = std::max(0.0f, terrain.height);
			    terrain.contrast = std::max(0.0f, terrain.contrast);
		    }
		    ComponentUI::end_region(general_open);

		    const bool noise_open = ComponentUI::start_region("Noise");
		    if (noise_open) {
			    ComponentUI::draw_enum_control("Type", terrain.noise_settings.type);
			    ComponentUI::draw_int_control("Seed", terrain.noise_settings.seed);
			    ComponentUI::draw_float_control("Frequency", terrain.noise_settings.frequency, 0.001f, "%.4f");
			    ComponentUI::draw_enum_control("Fractal", terrain.noise_settings.fractal);
			    ComponentUI::draw_int_control("Octaves", terrain.noise_settings.octaves);
			    ComponentUI::draw_float_control("Lacunarity", terrain.noise_settings.lacunarity, 0.01f, "%.3f");
			    ComponentUI::draw_float_control("Gain", terrain.noise_settings.gain, 0.01f, "%.3f");
			    ComponentUI::draw_float_control("Weighted Strength", terrain.noise_settings.weighted_strength, 0.01f, "%.3f");

			    terrain.noise_settings.frequency = std::max(0.0f, terrain.noise_settings.frequency);
			    terrain.noise_settings.octaves = std::max(1, terrain.noise_settings.octaves);
			    terrain.noise_settings.lacunarity = std::max(0.0f, terrain.noise_settings.lacunarity);
			    terrain.noise_settings.gain = std::max(0.0f, terrain.noise_settings.gain);
			    terrain.noise_settings.weighted_strength = std::max(0.0f, terrain.noise_settings.weighted_strength);
		    }
		    ComponentUI::end_region(noise_open);

		    const bool erosion_open = ComponentUI::start_region("Erosion");
		    if (erosion_open) {
			    ComponentUI::draw_int_control("Seed", terrain.erosion_settings.seed);
			    ComponentUI::draw_int_control("Droplet Count", terrain.erosion_settings.droplet_count);
			    ComponentUI::draw_int_control("Max Lifetime", terrain.erosion_settings.max_lifetime);
			    ComponentUI::draw_float_control(
			        "Sediment Capacity Factor",
			        terrain.erosion_settings.sediment_capacity_factor,
			        0.01f,
			        "%.3f");
			    ComponentUI::draw_float_control(
			        "Min Sediment Capacity",
			        terrain.erosion_settings.min_sediment_capacity,
			        0.001f,
			        "%.3f");
			    ComponentUI::draw_float_control("Erode Speed", terrain.erosion_settings.erode_speed, 0.01f, "%.3f");
			    ComponentUI::draw_float_control("Deposit Speed", terrain.erosion_settings.deposit_speed, 0.01f, "%.3f");
			    ComponentUI::draw_float_control("Evaporate Speed", terrain.erosion_settings.evaporate_speed, 0.001f, "%.3f");
			    ComponentUI::draw_float_control("Gravity", terrain.erosion_settings.gravity, 0.01f, "%.3f");
			    ComponentUI::draw_float_control("Inertia", terrain.erosion_settings.inertia, 0.01f, "%.3f");

			    terrain.erosion_settings.droplet_count = std::max(0, terrain.erosion_settings.droplet_count);
			    terrain.erosion_settings.max_lifetime = std::max(1, terrain.erosion_settings.max_lifetime);
			    terrain.erosion_settings.sediment_capacity_factor = std::max(0.0f, terrain.erosion_settings.sediment_capacity_factor);
			    terrain.erosion_settings.min_sediment_capacity = std::max(0.0f, terrain.erosion_settings.min_sediment_capacity);
			    terrain.erosion_settings.erode_speed = std::max(0.0f, terrain.erosion_settings.erode_speed);
			    terrain.erosion_settings.deposit_speed = std::max(0.0f, terrain.erosion_settings.deposit_speed);
			    terrain.erosion_settings.evaporate_speed = std::max(0.0f, terrain.erosion_settings.evaporate_speed);
			    terrain.erosion_settings.gravity = std::max(0.0f, terrain.erosion_settings.gravity);
			    terrain.erosion_settings.inertia = std::max(0.0f, terrain.erosion_settings.inertia);
		    }
		    ComponentUI::end_region(erosion_open);

		    const bool changed = terrain.noise_settings.type != before_noise_settings.type
		        || terrain.noise_settings.seed != before_noise_settings.seed
		        || terrain.noise_settings.frequency != before_noise_settings.frequency
		        || terrain.noise_settings.fractal != before_noise_settings.fractal
		        || terrain.noise_settings.octaves != before_noise_settings.octaves
		        || terrain.noise_settings.lacunarity != before_noise_settings.lacunarity
		        || terrain.noise_settings.gain != before_noise_settings.gain
		        || terrain.noise_settings.weighted_strength != before_noise_settings.weighted_strength
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
