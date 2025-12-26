#include "origo/scripting/ScriptSystem.h"
#include "origo/scripting/ScriptComponentRegistry.h"
#include <stdexcept>

namespace Origo {

void ScriptSystem::InitialiseState() {
	sol::table componentTable = s_Lua.create_named_table("Component");
	componentTable.set_function(
	    "define",
	    [](const std::string& name, sol::table fields) {
		    ScriptComponentRegistry::RegisterComponentFromLua(name, fields);
	    });
	s_Lua.open_libraries(sol::lib::base, sol::lib::io, sol::lib::math, sol::lib::table);
}

void ScriptSystem::Register(const UUID& id, std::string source) {
	for (auto& [existingId, _] : s_Scripts) {
		if (existingId == id)
			return;
	}
	s_Scripts.emplace_back(id, ScriptEntry { std::move(source) });
}

void ScriptSystem::ReloadAll() {
	s_Lua = sol::state {};
	InitialiseState();

	for (auto& [id, entry] : s_Scripts) {
		Execute(entry.Source, id.ToString());
	}
}

void ScriptSystem::Execute(
    const std::string& source,
    const std::string& debugName) {
	sol::load_result load = s_Lua.load(source, debugName);
	if (!load.valid()) {
		sol::error err = load;
		throw std::runtime_error(err.what());
	}

	sol::protected_function fn = load;
	sol::protected_function_result res = fn();
	if (!res.valid()) {
		sol::error err = res;
		throw std::runtime_error(err.what());
	}
}

}
