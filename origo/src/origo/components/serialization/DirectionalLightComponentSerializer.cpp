#include "DirectionalLightComponentSerializer.h"
#include "origo/components/DirectionalLight.h"
#include "origo/assets/AssetManager.h"

namespace Origo {

void DirectionalLightComponentSerializer::Serialize(Component* comp, ISerializer& backend) const {
	auto light = static_cast<DirectionalLightComponent*>(comp);

	backend.Write("ambient", light->GetAmbientFactor());
	backend.Write("intensity", light->GetIntensity());

	auto color = light->GetLightColor();
	backend.Write("color_r", color.r);
	backend.Write("color_g", color.g);
	backend.Write("color_b", color.b);
}

void DirectionalLightComponentSerializer::Deserialize(Component* comp, ISerializer& backend) {
	auto light = static_cast<DirectionalLightComponent*>(comp);

	float ambient = light->GetAmbientFactor();
	float intensity = light->GetIntensity();
	glm::vec3 color = light->GetLightColor();

	backend.TryRead("ambient", ambient);
	backend.TryRead("intensity", intensity);

	backend.TryRead("color_r", color.r);
	backend.TryRead("color_g", color.g);
	backend.TryRead("color_b", color.b);

	light->SetAmbientFactor(ambient);
	light->SetIntensity(intensity);
	light->SetLightColor(color);
}

}
