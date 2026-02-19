#include "PointLightSystem.h"

#include "origo/components/PointLight.h"
#include "origo/components/Transform.h"

#include "origo/renderer/RenderContext.h"

#include "origo/scene/SystemScheduler.h"

namespace Origo {
void PointLightSystem::Render(Origo::Scene* scene, RenderContext& rCtx) {
	scene->View<Origo::PointLightComponent, Origo::TransformComponent>(
	    [&](Origo::RID, Origo::PointLightComponent& light, Origo::TransformComponent& transform) {
		    scene->View<PointLightComponent, TransformComponent>(
		        [&](RID, PointLightComponent& light, TransformComponent& transform) {
			        PointLightData data;

			        data.position = transform.GetPosition();
			        data.color = light.Color;
			        data.intensity = light.Intensity;
			        data.constant = light.Constant;
			        data.linear = light.Linear;
			        data.quadratic = light.Quadratic;

			        rCtx.PushPointLight(data);
		        });
	    });
}

REGISTER_RENDER_SYSTEM(
    Origo::GamePhase::RenderAlways,
    Origo::PointLightSystem);

}
