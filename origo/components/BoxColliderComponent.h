#pragma once

#include "origo/components/Component.h"
#include "origo/core/Typedefs.h"

namespace Origo {

struct BoxColliderComponent : public Component {
	Vec3 Offset { 0.0f };
	Vec3 Size { 1.0f };

	std::string get_component_name() const { return "Box Collider Component"; }
};

}
