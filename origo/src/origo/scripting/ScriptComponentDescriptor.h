#pragma once

#include "origo/core/UUID.h"
#include "origo/scripting/ScriptComponentInstance.h"
#include "origo/scripting/ScriptFieldDescriptor.h"

namespace Origo {

struct ScriptComponentDescriptor {
	ScriptComponentID ID { UUID::generate() };
	UUID ScriptID { UUID::generate() };
	std::string Name { "" };
	std::vector<ScriptFieldDescriptor> Fields {};
};

}
