#pragma once

#include "nlohmann/json.hpp"

namespace Origo {
class Entity;

class Component {
public:
	Component(Ref<Entity> entity)
	    : AttachedTo(entity) { };
	virtual ~Component() = default;
	virtual std::string GetName() const = 0;

public:
	Ref<Entity> AttachedTo;
};

}
