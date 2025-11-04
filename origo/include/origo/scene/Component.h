#pragma once

namespace Origo {
class Entity;

class Component {
public:
	Component(Ref<Entity> entity = nullptr)
	    : AttachedTo(entity) { };
	virtual ~Component() = default;
	virtual std::string GetName() const = 0;

public:
	Ref<Entity> AttachedTo;
};

}
