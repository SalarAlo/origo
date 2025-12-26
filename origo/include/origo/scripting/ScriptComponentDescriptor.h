#pragma once

#include "origo/scripting/ScriptFieldDescriptor.h"

namespace Origo {

struct ScriptComponentDescriptor {
	std::string Name;
	std::vector<ScriptFieldDescriptor> Fields;
};

}
