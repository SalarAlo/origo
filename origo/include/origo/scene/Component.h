#pragma once

namespace Origo {
class Entity;

class Component {
public:
	Component(Entity* entity = nullptr)
	    : AttachedTo(entity) { };
	virtual ~Component() = default;
	virtual std::string GetName() const = 0;

public:
	Entity* AttachedTo;
};

}
