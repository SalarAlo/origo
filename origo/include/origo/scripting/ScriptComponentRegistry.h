#pragma once

#include "origo/core/Action.h"
#include "origo/core/UUID.h"
#include "origo/scripting/ScriptComponentDescriptor.h"
#include "origo/scripting/ScriptComponentInstance.h"
#include "sol/table.hpp"
#include <unordered_map>
#include <string>

namespace Origo {

class ScriptComponentRegistry {
public:
	static ScriptComponentID RegisterOrUpdate(ScriptComponentDescriptor descriptor);
	static ScriptComponentID RegisterComponentFromLua(const UUID& uuid, const std::string& name, sol::table fields);

	static const ScriptComponentDescriptor& Get(ScriptComponentID id);

	static std::optional<ScriptComponentID> TryFindByName(const std::string& name);
	static bool Exists(ScriptComponentID id);

	static const std::unordered_map<ScriptComponentID, ScriptComponentDescriptor>& GetAll() { return s_Descriptors; }
	static Action<void, ScriptComponentID> OnScriptComponentUpdated();

private:
	inline static std::unordered_map<ScriptComponentID, ScriptComponentDescriptor> s_Descriptors {};
	inline static std::unordered_map<std::string, ScriptComponentID> s_NameToScriptComponentID {};
};

}
