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
		    const auto before_settings = terrain.settings;
		    const float before_scale = terrain.scale;
		    const float before_contrast = terrain.contrast;

		    if (terrain.started_rebuilding)
			    ImGui::TextDisabled("Rebuilding...");

		    ImGui::BeginDisabled(terrain.should_rebuild);

		    const bool general_open = ComponentUI::start_region("Terrain");
		    if (general_open) {
			    ComponentUI::draw_float_control("Scale", terrain.scale);
			    ComponentUI::draw_float_control("Contrast", terrain.contrast);

			    terrain.scale = std::max(0.001f, terrain.scale);
			    terrain.contrast = std::max(0.0f, terrain.contrast);
		    }
		    ComponentUI::end_region(general_open);

		    const bool noise_open = ComponentUI::start_region("Noise");
		    if (noise_open) {
			    ComponentUI::draw_enum_control("Type", terrain.settings.type);
			    ComponentUI::draw_int_control("Seed", terrain.settings.seed);
			    ComponentUI::draw_float_control("Frequency", terrain.settings.frequency, 0.001f, "%.4f");
			    ComponentUI::draw_enum_control("Fractal", terrain.settings.fractal);
			    ComponentUI::draw_int_control("Octaves", terrain.settings.octaves);
			    ComponentUI::draw_float_control("Lacunarity", terrain.settings.lacunarity, 0.01f, "%.3f");
			    ComponentUI::draw_float_control("Gain", terrain.settings.gain, 0.01f, "%.3f");
			    ComponentUI::draw_float_control("Weighted Strength", terrain.settings.weighted_strength, 0.01f, "%.3f");

			    terrain.settings.frequency = std::max(0.0f, terrain.settings.frequency);
			    terrain.settings.octaves = std::max(1, terrain.settings.octaves);
			    terrain.settings.lacunarity = std::max(0.0f, terrain.settings.lacunarity);
			    terrain.settings.gain = std::max(0.0f, terrain.settings.gain);
			    terrain.settings.weighted_strength = std::max(0.0f, terrain.settings.weighted_strength);
		    }
		    ComponentUI::end_region(noise_open);

		    const bool changed = terrain.settings.type != before_settings.type
		        || terrain.settings.seed != before_settings.seed
		        || terrain.settings.frequency != before_settings.frequency
		        || terrain.settings.fractal != before_settings.fractal
		        || terrain.settings.octaves != before_settings.octaves
		        || terrain.settings.lacunarity != before_settings.lacunarity
		        || terrain.settings.gain != before_settings.gain
		        || terrain.settings.weighted_strength != before_settings.weighted_strength
		        || terrain.scale != before_scale
		        || terrain.contrast != before_contrast;

		    ImGui::EndDisabled();

		    if (changed)
			    terrain.should_rebuild = true;
	    });

	return true;
}();

}
