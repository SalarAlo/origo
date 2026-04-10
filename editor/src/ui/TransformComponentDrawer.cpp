#include <glm/vec3.hpp>

#include "origo/components/TransformComponent.h"

#include "ui/ComponentUI.h"
#include "ui/InspectorDrawRegistry.h"

namespace OrigoEditor {

static bool s_registered = []() {
	InspectorDrawRegistry::get_instance().register_native_drawer<Origo::TransformComponent>(
	    "Transform",
	    "./icons/TransformComponent.svg",
	    [](Origo::TransformComponent& t) {
		    glm::vec3 pos = t.get_position();
		    glm::vec3 rot = t.get_rotation();
		    glm::vec3 scl = t.get_scale();

		    ComponentUI::draw_vec3_control("Position", pos);
		    ComponentUI::draw_vec3_control("Rotation", rot);
		    ComponentUI::draw_vec3_control("Scale", scl);

		    t.set_position(pos);
		    t.set_rotation(rot);
		    t.set_scale(scl);
	    });

	return true;
}();

}
