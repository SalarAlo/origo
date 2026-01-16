
#pragma once

#include "Component.h"

namespace Origo {

class NameComponent : public Component {
public:
	NameComponent(std::string_view s = "some_name")
	    : m_Name(s) { };

	std::string GetComponentName() const override { return "Name"; }
	std::string GetName() const { return m_Name; }
	void SetName(std::string_view name) { m_Name = name; }

private:
	std::string m_Name;
};

}
