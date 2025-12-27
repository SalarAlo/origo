#pragma once

#include "origo/core/Action.h"
#include "origo/scripting/ScriptComponentDescriptor.h"
#include "origo/scripting/ScriptComponentInstance.h"
#include "sol/table.hpp"
#include <unordered_map>
#include <vector>
#include <string>

namespace Origo {

class ScriptComponentRegistry {
public:
	static ScriptComponentID RegisterOrUpdate(ScriptComponentDescriptor descriptor);
	static ScriptComponentID RegisterComponentFromLua(const std::string& name, sol::table fields);

	static const ScriptComponentDescriptor& Get(ScriptComponentID id);

	static std::optional<ScriptComponentID> TryFindByName(const std::string& name);
	static bool Exists(ScriptComponentID id);

	static const std::vector<ScriptComponentDescriptor>& GetAll() { return Descriptors(); }
	static Action<void, ScriptComponentID> OnScriptComponentUpdated();

private:
	static std::vector<ScriptComponentDescriptor>& Descriptors();
	static std::unordered_map<std::string, ScriptComponentID>& NameToID();
};

}
