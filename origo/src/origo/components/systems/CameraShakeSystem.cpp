#include "origo/components/systems/CameraShakeSystem.h"

#include "origo/components/CameraComponent.h"
#include "origo/components/CameraShakeComponent.h"

#include "origo/scene/GamePhase.h"
#include "origo/scene/SystemScheduler.h"

namespace Origo {
void CameraShakeSystem::update(Scene* scene, float dt) {
	scene->view<CameraComponent, CameraShakeComponent>(
	    [&](RID, CameraComponent& camera, CameraShakeComponent& shake) {
		    if (!camera.IsPrimary)
			    return;

		    shake.ShakeCounter += dt;
	    });
}
}
REGISTER_UPDATE_SYSTEM(
    Origo::GamePhase::UpdateGame,
    Origo::CameraShakeSystem);
