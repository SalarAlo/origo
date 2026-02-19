
#include "DirectionalLightSystem.h"

#include "origo/components/DirectionalLight.h"
#include "origo/components/DirectionalLightData.h"
#include "origo/components/Transform.h"

#include "origo/scene/GamePhase.h"
#include "origo/scene/SystemScheduler.h"

namespace Origo {

void DirectionalLightSystem::Render(Scene* scene, RenderContext& rCtx) {
	scene->View<DirectionalLightComponent, TransformComponent>(
	    [&](RID entity,
	        DirectionalLightComponent& light,
	        TransformComponent& transform) {
		    DirectionalLightData data {};

		    data.Direction = transform.GetForward();
		    data.Ambient = light.AmbientFactor;
		    data.Color = light.LightColor;
		    data.Intensity = light.Intensity;

		    rCtx.PushDirectionalLight(data);
	    });
}

}

REGISTER_RENDER_SYSTEM(
    Origo::GamePhase::RenderAlways,
    Origo::DirectionalLightSystem);
