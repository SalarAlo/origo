#pragma once

#include "origo/Camera.h"
#include "origo/scene/Component.h"
namespace Origo {

class CameraComponent : public Component {
public:
	bool IsPrimary;

public:
	std::string GetName() const override { return "Camera"; }
	Camera& GetCamera() { return m_Camera; }

private:
	Camera m_Camera { 1.0f };
};

}
