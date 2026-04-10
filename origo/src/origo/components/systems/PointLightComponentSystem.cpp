#include "PointLightComponentSystem.h"

#include "origo/components/PointLightComponent.h"
#include "origo/components/TransformComponent.h"

#include "origo/renderer/RenderContext.h"

#include "origo/scene/SystemScheduler.h"

namespace Origo {
void PointLightComponentSystem::render(Origo::Scene* scene, RenderContext& rCtx) {
	scene->view<Origo::PointLightComponent, Origo::TransformComponent>(
	    [&](Origo::RID, Origo::PointLightComponent& light, Origo::TransformComponent& transform) {
		    PointLightData data;

		    data.Position = transform.get_position();
		    data.Color = light.Color;
		    data.Intensity = light.Intensity;
		    data.Constant = light.Constant;
		    data.Linear = light.Linear;
		    data.Quadratic = light.Quadratic;

		    rCtx.push_point_light(data);
	    });
}

REGISTER_RENDER_SYSTEM(
    Origo::GamePhase::RenderAlways,
    Origo::PointLightComponentSystem);

}
