#pragma once

#include "origo/Camera.h"
#include "origo/scene/Component.h"
#include "origo/serialization/ISerializer.h"
namespace Origo {

class CameraComponent : public Component {
public:
	bool IsPrimary { false };

public:
	std::string GetComponentName() const override { return "Camera"; }
	Camera& GetCamera() { return m_Camera; }

	void Serialize(ISerializer& backend) const override;
	void Deserialize(ISerializer& backend) override;

private:
	Camera m_Camera { 1.0f };
};

}
