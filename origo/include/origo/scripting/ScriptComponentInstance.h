
#pragma once

#include "origo/scripting/Variant.h"

namespace Origo {
using ScriptComponentID = int;

struct ScriptComponentInstance {
	ScriptComponentID ID;
	std::vector<Variant> Values;
};

}
