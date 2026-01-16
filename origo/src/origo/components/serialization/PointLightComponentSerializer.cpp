#include "PointLightComponentSerializer.h"
#include "origo/components/PointLight.h"

namespace Origo {

void PointLightComponentSerializer::Serialize(Component* comp, ISerializer& s) const {
	auto light = static_cast<PointLightComponent*>(comp);

	s.Write("intensity", light->GetIntensity());

	auto color = light->GetLightColor();
	s.Write("color_r", color.r);
	s.Write("color_g", color.g);
	s.Write("color_b", color.b);

	s.Write("atten_constant", light->GetConstant());
	s.Write("atten_linear", light->GetLinear());
	s.Write("atten_quadratic", light->GetQuadratic());
}

void PointLightComponentSerializer::Deserialize(Component* comp, ISerializer& s) {
	auto light = static_cast<PointLightComponent*>(comp);

	float intensity = light->GetIntensity();
	glm::vec3 color = light->GetLightColor();
	float c = light->GetConstant();
	float l = light->GetLinear();
	float q = light->GetQuadratic();

	s.TryRead("intensity", intensity);

	s.TryRead("color_r", color.r);
	s.TryRead("color_g", color.g);
	s.TryRead("color_b", color.b);

	s.TryRead("atten_constant", c);
	s.TryRead("atten_linear", l);
	s.TryRead("atten_quadratic", q);

	light->SetIntensity(intensity);
	light->SetLightColor(color);
	light->SetAttenuation(c, l, q);
}

}
