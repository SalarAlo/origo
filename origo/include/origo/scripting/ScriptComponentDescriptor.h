#pragma once

#include "origo/core/UUID.h"
#include "origo/scripting/ScriptComponentInstance.h"
#include "origo/scripting/ScriptFieldDescriptor.h"

namespace Origo {

struct ScriptComponentDescriptor {
	ScriptComponentID ID;
	std::string Name;
	std::vector<ScriptFieldDescriptor> Fields;
};

}
