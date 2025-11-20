#include "ui/ComponentUI.h"
#include "origo/scene/Transform.h"
#include "ui/InspectorDrawableRegistry.h"
#include <glm/vec3.hpp>

namespace OrigoEditor {

static bool s_Registered = []() {
	InspectorDrawRegistry::Register<Origo::Transform>("Transform", [](Origo::Transform& t) {
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
