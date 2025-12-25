#pragma once

#include "origo/scene/Component.h"

namespace OrigoEditor {

class FallComponent : public Origo::Component {
public:
	FallComponent(float fallSpeed = 1.0f)
	    : m_FallSpeed(fallSpeed) { }

	float GetFallSpeed() const { return m_FallSpeed; }
	void SetFallSpeed(float fallSpeed) { m_FallSpeed = fallSpeed; }
	std::string GetName() const override { return "FallingComponent"; }

private:
	float m_FallSpeed;
};

}
