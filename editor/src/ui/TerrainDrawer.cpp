#include <glm/vec3.hpp>

#include "origo/components/TerrainComponent.h"

#include "ui/ComponentUI.h"
#include "ui/InspectorDrawRegistry.h"

namespace OrigoEditor {

static bool s_registered = []() {
	InspectorDrawRegistry::get_instance().register_native_drawer<Origo::TerrainComponent>(
	    "Terrain",
	    "./icons/TransformComponent.svg",
	    [](Origo::TerrainComponent& t) {
		    ComponentUI::draw_float_control("Frequency", t.Frequency);
		    ComponentUI::draw_float_control("Amplitude", t.Amplitude);
		    ComponentUI::draw_int_control("Octaves", t.Octaves);
		    ComponentUI::draw_int_control("Size", t.Size);
		    ComponentUI::draw_float_control("Scale", t.Scale);
	    });

	return true;
}();

}
