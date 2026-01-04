#pragma once

namespace Origo {

class Component {
public:
	Component() = default;
	virtual ~Component() = default;
	virtual std::string GetName() const = 0;
};

}
