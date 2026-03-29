#pragma once

#include "origo/components/Component.h"

namespace Origo {
struct CameraShakeComponent : public Component {
	float ShakeDuration { 1.0 };
	float ShakeDelay { 0.0 };

	float ShakeAmplitude { 1.0 };
	float ShakeFrequency { 1.0 };

	float ShakeCounter {};

	std::string get_component_name() const { return "camera shake component"; }
};

}
