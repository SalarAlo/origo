
#include "origo/scene/DirectionalLightSystem.h"
#include "origo/assets/AssetManagerFast.h"
#include "origo/assets/Shader.h"
#include "origo/scene/GamePhase.h"
#include "origo/scene/DirectionalLight.h"
#include "origo/scene/SystemScheduler.h"
#include "origo/scene/Transform.h"

namespace Origo {

void DirectionalLightSystem::Update(Origo::Scene* scene, float dt) {
	scene->View<DirectionalLight, Transform>(
	    [&](RID entity,
	        DirectionalLight& light,
	        Transform& transform) {
		    auto shaderHandle { light.GetShaderTarget() };
		    if (!shaderHandle.has_value()) {
			    return;
		    }
		    auto shader { AssetManager::GetInstance().Get<Shader>(*shaderHandle) };

		    if (!shader)
			    return;

		    shader->SetUniform(
		        "u_DirLight.direction",
		        transform.GetForward());

		    shader->SetUniform(
		        "u_DirLight.color",
		        light.GetLightColor());

		    shader->SetUniform<float>(
		        "u_DirLight.intensity",
		        light.GetIntensity());

		    shader->SetUniform("u_Ambient", light.GetAmbientFactor());
	    });
}

}

REGISTER_UPDATE_SYSTEM(
    Origo::GamePhase::UpdatePresentation,
    Origo::DirectionalLightSystem);
