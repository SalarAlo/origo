
#include "origo/scene/LightSystem.h"
#include "origo/assets/AssetManagerFast.h"
#include "origo/assets/Shader.h"
#include "origo/scene/GamePhase.h"
#include "origo/scene/Light.h"
#include "origo/scene/SystemScheduler.h"
#include "origo/scene/Transform.h"

namespace Origo {

void LightSystem::Update(Origo::Scene* scene, float dt) {
	scene->View<Light, Transform>(
	    [&](RID entity,
	        Light& light,
	        Transform& transform) {
		    auto shaderHandle { light.GetShaderTarget() };
		    if (!shaderHandle.has_value()) {
			    return;
		    }
		    auto shader { AssetManager::GetInstance().Get<Shader>(*shaderHandle) };

		    if (!shader)
			    return;

		    shader->SetUniform("u_LightPos", transform.GetPosition());
		    shader->SetUniform("u_Ambient", light.GetAmbientFactor());
		    shader->SetUniform("u_LightColor", light.GetLightColor());
		    shader->SetUniform("u_ShinyFactor", light.GetShinyFactor());
	    });
}

}

REGISTER_UPDATE_SYSTEM(
    Origo::GamePhase::UpdatePresentation,
    Origo::LightSystem);
