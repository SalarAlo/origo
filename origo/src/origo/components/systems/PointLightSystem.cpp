#include "PointLightSystem.h"

#include "origo/renderer/RenderContext.h"
#include "origo/scene/SystemScheduler.h"
#include "origo/components/PointLight.h"
#include "origo/components/Transform.h"

namespace Origo {
void PointLightSystem::Render(Origo::Scene* scene, RenderContext& rCtx) {
	scene->View<Origo::PointLightComponent, Origo::TransformComponent>(
	    [&](Origo::RID, Origo::PointLightComponent& light, Origo::TransformComponent& transform) {
		    scene->View<PointLightComponent, TransformComponent>(
		        [&](RID, PointLightComponent& light, TransformComponent& transform) {
			        PointLightData data;

			        data.position = transform.GetPosition();
			        data.color = light.GetLightColor();
			        data.intensity = light.GetIntensity();
			        data.constant = light.GetConstant();
			        data.linear = light.GetLinear();
			        data.quadratic = light.GetQuadratic();

			        rCtx.PushPointLight(data);
		        });
	    });
}

REGISTER_RENDER_SYSTEM(
    Origo::GamePhase::RenderGeometry,
    Origo::PointLightSystem);

}
