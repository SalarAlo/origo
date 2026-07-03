#pragma once

#include <string>
#include <vector>

#include "origo/scripting/FieldDescriptor.h"

namespace Origo {

struct ReflectedFieldValue {
	FieldID ID { UUID::bad() };
	std::string Name {};
	TypeRef Type {};
	Variant Value { 0 };
};

inline ReflectedFieldValue make_reflected_field_value(const FieldDescriptor& field) {
	return ReflectedFieldValue {
		.ID = field.ID,
		.Name = field.Name,
		.Type = field.Type,
		.Value = field.DefaultValue
	};
}

using ReflectedFieldStorage = std::vector<ReflectedFieldValue>;

}
