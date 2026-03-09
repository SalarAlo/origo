#include <glm/vec3.hpp>

#include "origo/components/BoxColliderComponent.h"

#include "ui/InspectorDrawRegistry.h"

namespace OrigoEditor {

static bool s_registered = []() {
	InspectorDrawRegistry::get_instance().register_native_drawer<Origo::BoxColliderComponent>(
	    "BoxCollider",
	    "icons/Entity.svg",
	    [](Origo::BoxColliderComponent& box) {
	    });

	return true;
}();

}
