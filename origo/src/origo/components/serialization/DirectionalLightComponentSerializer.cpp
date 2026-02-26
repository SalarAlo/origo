#include "DirectionalLightComponentSerializer.h"

#include "origo/components/DirectionalLight.h"

namespace Origo {

void DirectionalLightComponentSerializer::serialize(Component* comp, ISerializer& backend) const {
	auto light = static_cast<DirectionalLightComponent*>(comp);

	backend.write("ambient", light->AmbientFactor);
	backend.write("intensity", light->Intensity);

	const auto& color = light->LightColor;
	backend.write("color_r", color.r);
	backend.write("color_g", color.g);
	backend.write("color_b", color.b);
}

void DirectionalLightComponentSerializer::deserialize(Component* comp, ISerializer& backend) {
	auto light = static_cast<DirectionalLightComponent*>(comp);

	backend.try_read("ambient", light->AmbientFactor);
	backend.try_read("intensity", light->Intensity);

	auto& color = light->LightColor;
	backend.try_read("color_r", color.r);
	backend.try_read("color_g", color.g);
	backend.try_read("color_b", color.b);
}

}
