
#pragma once

#include "Component.h"

namespace Origo {

class NameComponent : public Component {
public:
	NameComponent(std::string_view name = "x")
	    : Name(name) { };

	std::string GetComponentName() const override { return "Name"; }

	std::string Name;
};

}
