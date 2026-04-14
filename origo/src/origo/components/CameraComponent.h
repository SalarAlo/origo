#pragma once

#include "Component.h"

#include "origo/Camera.h"
#include "origo/renderer/PostProcessSettings.h"

namespace Origo {

class CameraComponent : public Component {
public:
	bool IsPrimary { false };
	Camera CameraObj { 1.0f };
	PostProcessSettings PostProcess {};

public:
	std::string get_component_name() const override { return "Camera"; }
};

}
