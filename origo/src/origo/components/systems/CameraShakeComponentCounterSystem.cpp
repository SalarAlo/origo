#include "origo/components/systems/CameraShakeComponentCounterSystem.h"

#include "origo/components/CameraComponent.h"
#include "origo/components/CameraShakeComponent.h"

#include "origo/scene/SystemScheduler.h"

namespace Origo {

void CameraShakeComponentCounterSystem::update(Scene* scene, float dt) {
	scene->view<CameraShakeComponent, CameraComponent>([&](RID _, CameraShakeComponent& camera_shake, CameraComponent& camera) {
		if (!camera.IsPrimary)
			return;

		camera_shake.Counter += dt;
	});
}

}

REGISTER_UPDATE_SYSTEM(
    Origo::GamePhase::UpdateGame,
    Origo::CameraShakeComponentCounterSystem)
