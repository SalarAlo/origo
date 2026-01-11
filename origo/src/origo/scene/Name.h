
#pragma once

#include "origo/scene/Component.h"
#include "origo/serialization/ISerializer.h"

namespace Origo {

class Name : public Component {
public:
	Name(std::string_view s = "some_name")
	    : m_Name(s) { };

	std::string GetComponentName() const override { return "Name"; }
	std::string GetName() const { return m_Name; }
	void SetName(std::string_view name) { m_Name = name; }

	void Serialize(ISerializer& backend) const override {
		backend.Write("name", m_Name);
	}
	void Deserialize(ISerializer& backend) override {
		backend.TryRead("name", m_Name);
	}

private:
	std::string m_Name;
};

}
