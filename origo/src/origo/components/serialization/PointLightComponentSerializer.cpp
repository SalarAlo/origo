#include "PointLightComponentSerializer.h"

#include "origo/components/PointLight.h"

namespace Origo {

void PointLightComponentSerializer::Serialize(Component* comp, ISerializer& s) const {
	auto light = static_cast<PointLightComponent*>(comp);

	s.Write("intensity", light->Intensity);

	const auto& color = light->Color;

	s.Write("color_r", color.r);
	s.Write("color_g", color.g);
	s.Write("color_b", color.b);

	s.Write("atten_constant", light->Constant);
	s.Write("atten_linear", light->Linear);
	s.Write("atten_quadratic", light->Quadratic);
}

void PointLightComponentSerializer::Deserialize(Component* comp, ISerializer& s) {
	auto light = static_cast<PointLightComponent*>(comp);

	s.TryRead("intensity", light->Intensity);

	s.TryRead("color_r", light->Color.r);
	s.TryRead("color_g", light->Color.g);
	s.TryRead("color_b", light->Color.b);

	s.TryRead("atten_constant", light->Constant);
	s.TryRead("atten_linear", light->Linear);
	s.TryRead("atten_quadratic", light->Quadratic);
}

}
