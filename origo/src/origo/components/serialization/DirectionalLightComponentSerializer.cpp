#include "DirectionalLightComponentSerializer.h"

#include "origo/components/DirectionalLight.h"

namespace Origo {

void DirectionalLightComponentSerializer::Serialize(Component* comp, ISerializer& backend) const {
	auto light = static_cast<DirectionalLightComponent*>(comp);

	backend.Write("ambient", light->AmbientFactor);
	backend.Write("intensity", light->Intensity);

	const auto& color = light->LightColor;
	backend.Write("color_r", color.r);
	backend.Write("color_g", color.g);
	backend.Write("color_b", color.b);
}

void DirectionalLightComponentSerializer::Deserialize(Component* comp, ISerializer& backend) {
	auto light = static_cast<DirectionalLightComponent*>(comp);

	backend.TryRead("ambient", light->AmbientFactor);
	backend.TryRead("intensity", light->Intensity);

	auto& color = light->LightColor;
	backend.TryRead("color_r", color.r);
	backend.TryRead("color_g", color.g);
	backend.TryRead("color_b", color.b);
}

}
