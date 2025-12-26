#pragma once

#include "origo/scripting/ScriptComponentDescriptor.h"
#include "origo/scripting/ScriptComponentInstance.h"
#include "sol/table.hpp"
#include <unordered_map>
#include <vector>
#include <string>

namespace Origo {

class ScriptComponentRegistry {
public:
	static ScriptComponentID Register(ScriptComponentDescriptor descriptor);
	static ScriptComponentID RegisterComponentFromLua(const std::string& name, sol::table fields);

	static const ScriptComponentDescriptor& Get(ScriptComponentID id);

	static ScriptComponentID FindByName(const std::string& name);
	static bool Exists(ScriptComponentID id);

	static const std::vector<ScriptComponentDescriptor>& GetAll() { return Descriptors(); }

private:
	static std::vector<ScriptComponentDescriptor>& Descriptors();
	static std::unordered_map<std::string, ScriptComponentID>& NameToID();
};

}
