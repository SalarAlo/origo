
#pragma once

#include "Component.h"

namespace Origo {

class NameComponent : public Component {
public:
	NameComponent(std::string_view name = "x")
	    : Name(name) { };

	std::string get_component_name() const override { return "Name"; }

	std::string Name;
};

}
