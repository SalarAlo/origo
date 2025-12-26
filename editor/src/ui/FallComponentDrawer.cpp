#include "components/FallComponent.h"
#include "ui/ComponentUI.h"
#include "ui/InspectorDrawRegistry.h"

#include <glm/vec3.hpp>

namespace OrigoEditor {

static bool s_Registered = []() {
	InspectorDrawRegistry::RegisterNativeDrawer<FallComponent>("Fall", "icons/Fall.svg", [](FallComponent& fall) {
		auto fs { fall.GetFallSpeed() };
		ComponentUI::DrawFloatControl("Fall Speed", fs);
		fall.SetFallSpeed(fs);
	});

	return true;
}();

}
