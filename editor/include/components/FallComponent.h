#pragma once

#include "origo/scene/Component.h"

namespace OrigoEditor {

class FallComponent : public Origo::Component {
public:
	FallComponent(float fallSpeed)
	    : m_FallSpeed(fallSpeed) { }

	float GetFallSpeed() const { return m_FallSpeed; }
	std::string GetName() const override { return "FallingComponent"; }

private:
	float m_FallSpeed;
};

}
