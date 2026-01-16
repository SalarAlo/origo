#pragma once

namespace Origo {

class Component {
public:
	Component() = default;
	virtual ~Component() = default;
	virtual std::string GetComponentName() const = 0;
};

}
