#pragma once

#include "origo/components/Component.h"

namespace Origo {

struct BoxColliderComponent : public Component {
	std::string get_component_name() const { return "Box Collider Component"; }
};

}
