#include "PointLightSystem.h"
#include "origo/scene/SystemScheduler.h"

namespace Origo {
void PointLightSystem::Update(Origo::Scene* scene, float dt) {
	scene->View<Origo::PointLight, Origo::Transform>(
	    [&](Origo::RID, Origo::PointLight& light, Origo::Transform& transform) {
		    auto shaderHandle = light.GetShaderTarget();
		    if (!shaderHandle.has_value())
			    return;

		    auto shader = Origo::AssetManager::GetInstance().Get<Origo::Shader>(*shaderHandle);
		    if (!shader)
			    return;

		    shader->SetUniform("u_PointLightCount", 1);

		    shader->SetUniform("u_PointLights[0].position",
		        transform.GetPosition());
		    shader->SetUniform("u_PointLights[0].color",
		        light.GetLightColor());
		    shader->SetUniform("u_PointLights[0].intensity",
		        light.GetIntensity());

		    shader->SetUniform("u_PointLights[0].constant",
		        light.Constant());
		    shader->SetUniform("u_PointLights[0].linear",
		        light.Linear());
		    shader->SetUniform("u_PointLights[0].quadratic",
		        light.Quadratic());
	    });
}
REGISTER_UPDATE_SYSTEM(
    Origo::GamePhase::UpdatePresentation,
    Origo::PointLightSystem);
}
