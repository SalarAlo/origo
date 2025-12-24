#include "systems/FallSystem.h"
#include "components/FallComponent.h"
#include "origo/scene/SystemScheduler.h"
#include "origo/scene/Transform.h"

using namespace Origo;

namespace OrigoEditor {

void FallSystem::Update(Origo::Scene* scene, float dt) {
	scene->View<FallComponent, Transform>(
	    [&](RID entity,
	        FallComponent& faller,
	        Transform& transform) {
		    transform.SetPosition(transform.GetPosition() - transform.GetUp() * (dt * faller.GetFallSpeed()));
	    });
}

}

REGISTER_UPDATE_SYSTEM(
    Origo::GamePhase::Update,
    OrigoEditor::FallSystem)
