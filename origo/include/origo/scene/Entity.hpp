#pragma once

#include "Component.h"
#include "origo/core/RID.h"

namespace Origo {

class Entity {
public:
	Entity(std::string_view name)
	    : m_Name(name) {
	}

	const std::string& GetName() const {
		return m_Name;
	}

	RID GetId() { return m_Id; }

private:
	std::string m_Name {};
	RID m_Id { RID::New() };
};

}
