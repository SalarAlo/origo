#pragma once

#include "origo/serialization/ISerializer.h"
namespace Origo {

class Component {
public:
	Component() = default;
	virtual ~Component() = default;
	virtual std::string GetComponentName() const = 0;

	virtual void Serialize(ISerializer& s) const { };
	virtual void Deserialize(ISerializer& s) { };
};

}
