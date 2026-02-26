#pragma once

#include "Component.h"

#include "origo/Camera.h"

namespace Origo {

class CameraComponent : public Component {
public:
	bool IsPrimary { false };
	Camera CameraObj { 1.0f };

public:
	std::string get_component_name() const override { return "Camera"; }
};

}
