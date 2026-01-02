#include "components/EditorOutline.h"
#include "ui/ComponentUI.h"
#include "ui/InspectorDrawRegistry.h"
#include <glm/vec3.hpp>

namespace OrigoEditor {

static bool s_Registered = []() {
	InspectorDrawRegistry::RegisterNativeDrawer<EditorOutline>("Editor Selection", "icons/Paint.svg", [](EditorOutline& selection) {
		ComponentUI::DrawBoolControl("Should Outline", selection.ShouldOutline);
	});

	return true;
}();

}
