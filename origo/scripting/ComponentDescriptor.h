#pragma once

#include <string>
#include <vector>

#include "origo/scripting/FieldDescriptor.h"

namespace Origo {

using ComponentTypeID = TypeID;

struct ComponentDescriptor {
	ComponentTypeID ID { UUID::bad() };
	UUID AssetID { UUID::bad() };
	std::string Name {};
	uint32_t Version {};
	std::vector<FieldDescriptor> Fields {};
	std::vector<AnnotationMetadata> Annotations {};
};

}
