#include "PointLightComponentSerializer.h"

#include "origo/components/PointLight.h"

namespace Origo {

void PointLightComponentSerializer::serialize(Component* comp, ISerializer& s) const {
	auto light = static_cast<PointLightComponent*>(comp);

	s.write("intensity", light->Intensity);

	const auto& color = light->Color;

	s.write("color_r", color.r);
	s.write("color_g", color.g);
	s.write("color_b", color.b);

	s.write("atten_constant", light->Constant);
	s.write("atten_linear", light->Linear);
	s.write("atten_quadratic", light->Quadratic);
}

void PointLightComponentSerializer::deserialize(Component* comp, ISerializer& s) {
	auto light = static_cast<PointLightComponent*>(comp);

	s.try_read("intensity", light->Intensity);

	s.try_read("color_r", light->Color.r);
	s.try_read("color_g", light->Color.g);
	s.try_read("color_b", light->Color.b);

	s.try_read("atten_constant", light->Constant);
	s.try_read("atten_linear", light->Linear);
	s.try_read("atten_quadratic", light->Quadratic);
}

}
