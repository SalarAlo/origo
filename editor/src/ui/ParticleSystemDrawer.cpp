
#include <glm/vec3.hpp>

#include "origo/components/ParticleSystemComponent.h"

#include "ui/ComponentUI.h"
#include "ui/InspectorDrawRegistry.h"

namespace OrigoEditor {

static bool s_Registered = []() {
	InspectorDrawRegistry::RegisterNativeDrawer<Origo::ParticleSystemComponent>("Particle System", "./icons/ParticleSystem.svg", [](Origo::ParticleSystemComponent& ps) {
		ComponentUI::DrawBoolControl("Is Looping", ps.IsLooping);
		ComponentUI::DrawFloatControl("Spawn Speed", ps.SpawnSpeed);
		ComponentUI::DrawVec3Control("Start Size", ps.StartSize);
	});

	return true;
}();

}
