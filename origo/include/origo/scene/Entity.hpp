#pragma once

#include "Component.h"
#include "origo/core/Identifiable.h"

namespace Origo {

class Entity : public Identifiable {
public:
	Entity(std::string_view name)
	    : m_Name(name) {
	}

	const std::string& GetName() const {
		return m_Name;
	}

private:
	std::string m_Name {};
};

}
