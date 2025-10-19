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

	virtual void Awake() { };
	virtual void Start() { };
	virtual void Update() { };

public:
	Ref<Entity> AttachedTo;
};

}
