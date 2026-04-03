#pragma once

#include "origo/components/Component.h"

namespace Origo {

struct CameraShakeComponent : Component {
	std::string get_component_name() const { return "CameraShakeComponent"; }

	float Delay {};
	float Duration {};
	float Frequency {};
	float Amplitude {};

	float Counter {};
};

}
