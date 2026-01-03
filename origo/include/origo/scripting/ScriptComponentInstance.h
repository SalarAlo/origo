
#pragma once

#include "origo/core/UUID.h"
#include "origo/scripting/Variant.h"

namespace Origo {
using ScriptComponentID = UUID;
using ScriptComponentFieldID = UUID;

struct ScriptComponentFieldInstance {
	ScriptComponentFieldID ID;
	std::string Name;
	Variant Value;
};

struct ScriptComponentInstance {
	ScriptComponentID ID;
	std::vector<ScriptComponentFieldInstance> Fields;
};

}
