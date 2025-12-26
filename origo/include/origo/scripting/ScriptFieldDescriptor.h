#pragma once

#include "origo/scripting/Variant.h"

namespace Origo {

struct ScriptFieldDescriptor {
	std::string Name;
	VariantType Type;
	Variant DefaultValue;
};
}
