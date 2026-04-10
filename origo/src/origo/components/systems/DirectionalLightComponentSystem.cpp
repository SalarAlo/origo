
#include "DirectionalLightComponentSystem.h"

#include "origo/components/DirectionalLightComponent.h"
#include "origo/components/DirectionalLightData.h"
#include "origo/components/TransformComponent.h"

#include "origo/scene/GamePhase.h"
#include "origo/scene/SystemScheduler.h"

namespace Origo {

void DirectionalLightComponentSystem::render(Scene* scene, RenderContext& rCtx) {
	scene->view<DirectionalLightComponent, TransformComponent>(
	    [&](RID entity,
	        DirectionalLightComponent& light,
	        TransformComponent& transform) {
		    DirectionalLightData data {};

		    data.Direction = transform.get_forward();
		    data.Ambient = light.AmbientFactor;
		    data.Color = light.LightColor;
		    data.Intensity = light.Intensity;

		    rCtx.push_directional_light(data);
	    });
}

}

REGISTER_RENDER_SYSTEM(
    Origo::GamePhase::RenderAlways,
    Origo::DirectionalLightComponentSystem);
