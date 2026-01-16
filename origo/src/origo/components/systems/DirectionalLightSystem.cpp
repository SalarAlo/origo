
#include "DirectionalLightSystem.h"
#include "origo/components/DirectionalLightData.h"
#include "origo/scene/GamePhase.h"
#include "origo/components/DirectionalLight.h"
#include "origo/scene/SystemScheduler.h"
#include "origo/components/Transform.h"

namespace Origo {

void DirectionalLightSystem::Render(Scene* scene, RenderContext& rCtx) {
	scene->View<DirectionalLightComponent, TransformComponent>(
	    [&](RID entity,
	        DirectionalLightComponent& light,
	        TransformComponent& transform) {
		    DirectionalLightData data {};

		    data.Direction = transform.GetForward();
		    data.Ambient = light.GetAmbientFactor();
		    data.Color = light.GetLightColor();
		    data.Intensity = light.GetIntensity();

		    rCtx.PushDirectionalLight(data);
	    });
}

}

REGISTER_RENDER_SYSTEM(
    Origo::GamePhase::RenderGeometry,
    Origo::DirectionalLightSystem);
