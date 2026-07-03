#pragma once

#include <string>
#include <vector>

#include "origo/scripting/ReflectionMetadata.h"

namespace Origo {

struct FieldDescriptor {
	FieldID ID { UUID::bad() };
	std::string Name {};
	TypeRef Type {};
	Variant DefaultValue { 0 };
	std::vector<AnnotationMetadata> Annotations {};
	bool Serialized { true };
	bool InspectorVisible { true };
};

}
