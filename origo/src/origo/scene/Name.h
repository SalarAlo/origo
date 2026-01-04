
#pragma once

#include "origo/scene/Component.h"

namespace Origo {

class Name : public Component {
public:
	Name(std::string_view s = "some_name")
	    : m_Name(s) { };
	std::string GetName() const { return m_Name; }
	void SetName(std::string_view name) { m_Name = name; }

private:
	std::string m_Name;
};

}
