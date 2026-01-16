#include "ui/ComponentUI.h"
#include "origo/components/Transform.h"
#include "ui/InspectorDrawRegistry.h"
#include <glm/vec3.hpp>

namespace OrigoEditor {

static bool s_Registered = []() {
	InspectorDrawRegistry::RegisterNativeDrawer<Origo::TransformComponent>("Transform", "./icons/TransformComponent.svg", [](Origo::TransformComponent& t) {
		glm::vec3 pos = t.GetPosition();
		glm::vec3 rot = t.GetRotation();
		glm::vec3 scl = t.GetScale();

		ComponentUI::DrawVec3Control("Position", pos);
		ComponentUI::DrawVec3Control("Rotation", rot);
		ComponentUI::DrawVec3Control("Scale", scl);

		t.SetPosition(pos);
		t.SetRotation(rot);
		t.SetScale(scl);
	});

	return true;
}();

}
