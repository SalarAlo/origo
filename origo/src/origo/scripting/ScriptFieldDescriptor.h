#pragma once

#include "origo/scripting/ScriptComponentInstance.h"
#include "origo/scripting/Variant.h"

namespace Origo {

struct ScriptFieldDescriptor {
	ScriptComponentFieldID ID;
	std::string Name;
	VariantType Type;
	Variant DefaultValue;
};
}
