#pragma once

#include "origo/scripting/ComponentRegistry.h"
#include "origo/scripting/EnumRegistry.h"
#include "origo/scripting/ScriptRegistry.h"
#include "origo/scripting/StructRegistry.h"

namespace Origo {

class TypeRegistry {
public:
	static void replace_module(const NoctModuleMetadata& metadata);
};

}
