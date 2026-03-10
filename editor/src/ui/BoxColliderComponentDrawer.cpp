#include <glm/vec3.hpp>

#include "origo/components/BoxColliderComponent.h"

#include "ui/ComponentUI.h"
#include "ui/InspectorDrawRegistry.h"

namespace OrigoEditor {

static bool s_registered = []() {
	InspectorDrawRegistry::get_instance().register_native_drawer<Origo::BoxColliderComponent>(
	    "BoxCollider",
	    "icons/Entity.svg",
	    [](Origo::BoxColliderComponent& box) {
		    glm::vec3 offset = box.Offset;
		    glm::vec3 size = box.Size;

		    ComponentUI::draw_vec3_control("Offset", offset);
		    ComponentUI::draw_vec3_control("Size", size);

		    box.Offset = offset;
		    box.Size = size;
	    });

	return true;
}();

}
