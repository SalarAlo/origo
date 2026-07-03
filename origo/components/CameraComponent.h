#pragma once

#include "Component.h"

#include "origo/Camera.h"
#include "origo/renderer/PostProcessSettings.h"

namespace Origo {

class CameraComponent : public Component {
public:
	bool IsPrimary { false };
	Camera CameraObj { 1.0f };
	Vec3 BackgroundColor { 0.04f, 0.04f, 0.067f };
	PostProcessSettings PostProcess {};

public:
	std::string get_component_name() const override { return "Camera"; }
};

}
