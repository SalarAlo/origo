#pragma once

namespace Origo {
class Entity;

class Component {
public:
	Component() = default;
	virtual ~Component() = default;
	virtual std::string GetName() const = 0;
};

}
