#pragma once

namespace Origo {
class Entity;

class Component {
public:
	Component(Entity& entity)
	    : E(entity) { };
	virtual ~Component() = default;

public:
	Entity& E;
};
}
