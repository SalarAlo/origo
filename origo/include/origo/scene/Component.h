#pragma once

#include "nlohmann/json.hpp"

namespace Origo {
class Entity;

class Component {
public:
	Component(Entity& entity)
	    : E(entity) { };
	virtual ~Component() = default;

	virtual nlohmann::json Serialize() const = 0;
	virtual void Deserialize(const nlohmann::json& j) = 0;
	virtual std::string GetName() const = 0;

public:
	Entity& E;
};

}

#define COMP_SERI_FWD                              \
	std::string GetName() const override;      \
	nlohmann::json Serialize() const override; \
	void Deserialize(const nlohmann::json& j) override;

#define COMP_SERI_NAME_DEF(NAME) \
	std::string NAME::GetName() const { return #NAME; }
