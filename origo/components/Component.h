#pragma once

namespace Origo {

class Component {
public:
	Component() = default;
	virtual ~Component() = default;

	virtual std::string get_component_name() const = 0;
};

}
