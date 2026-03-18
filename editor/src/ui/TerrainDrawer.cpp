#include <glm/vec3.hpp>

#include "imgui.h"

#include "origo/components/TerrainComponent.h"

#include "ui/ComponentUI.h"
#include "ui/InspectorDrawRegistry.h"

namespace OrigoEditor {

static bool s_registered = []() {
	InspectorDrawRegistry::get_instance().register_native_drawer<Origo::TerrainComponent>(
	    "Terrain",
	    "./icons/TransformComponent.svg",
	    [](Origo::TerrainComponent& t) {
		    const auto previous_perlin = t.Perlin;
		    const int previous_size = t.Size;
		    const float previous_detail = t.Detail;
		    const float previous_height = t.Height;

		    const bool shape_open = ComponentUI::start_region("Shape", true);
		    if (shape_open) {
			    ComponentUI::draw_int_control("Size", t.Size);
			    ComponentUI::draw_float_control("Detail", t.Detail);
			    ComponentUI::draw_float_control("Height", t.Height);

			    const int width = t.Detail > 0.0f
			        ? static_cast<int>(t.Size / t.Detail) + 1
			        : 0;
			    const int quads = width > 1 ? (width - 1) * (width - 1) : 0;

			    ImGui::Spacing();
			    ImGui::SeparatorText("Preview");
			    ImGui::TextDisabled("Resolution: %d x %d vertices", width, width);
			    ImGui::TextDisabled("Surface: %d quads", quads);
			    ImGui::TextDisabled("Vertical span: %.2f units", t.Height * t.Perlin.Amplitude);
			    ComponentUI::end_region(shape_open);
		    }

		    const bool noise_open = ComponentUI::start_region("Noise", true);
		    if (noise_open) {
			    ComponentUI::draw_int_control("Seed", t.Perlin.Seed);
			    ComponentUI::draw_float_control("Frequency", t.Perlin.Frequency);
			    ComponentUI::draw_float_control("Amplitude", t.Perlin.Amplitude);
			    ComponentUI::draw_int_control("Octaves", t.Perlin.Octaves);
			    ComponentUI::draw_float_control("Scale", t.Perlin.Scale);
			    ComponentUI::draw_float_control("Persistence", t.Perlin.Persistence);
			    ComponentUI::draw_float_control("Lacunarity", t.Perlin.Lacunarity);
			    ComponentUI::end_region(noise_open);
		    }

		    const bool perlin_changed = previous_perlin.Frequency != t.Perlin.Frequency
		        || previous_perlin.Amplitude != t.Perlin.Amplitude
		        || previous_perlin.Octaves != t.Perlin.Octaves
		        || previous_perlin.Scale != t.Perlin.Scale
		        || previous_perlin.Persistence != t.Perlin.Persistence
		        || previous_perlin.Lacunarity != t.Perlin.Lacunarity
		        || previous_perlin.Seed != t.Perlin.Seed;

		    if (perlin_changed || previous_size != t.Size || previous_detail != t.Detail || previous_height != t.Height)
			    t.RebuildNecessary = true;
	    });

	return true;
}();

}
