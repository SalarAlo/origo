#include "origo/scripting/ScriptSystem.h"
#include "origo/scripting/ScriptComponentRegistry.h"
#include <stdexcept>

namespace Origo {

void ScriptSystem::InitialiseState() {
	if (s_Initialised)
		return;

	s_Initialised = true;
	sol::table componentTable = s_Lua.create_named_table("Component");

	componentTable.set_function(
	    "define",
	    [](const std::string& name, sol::table fields) {
		    ScriptComponentRegistry::RegisterComponentFromLua(name, fields);
	    });

	s_Lua.open_libraries(sol::lib::base, sol::lib::io, sol::lib::math, sol::lib::table);
}

void ScriptSystem::Register(const UUID& id, const std::filesystem::path& path, const std::string& source) {
	if (s_Scripts.contains(id)) {
		auto& entry = s_Scripts[id];
		entry.Path = path;
		entry.Source = source;
		entry.ReloadNecessary = true;

		return;
	}

	ScriptEntry entry {};
	entry.Source = source;
	entry.Path = path;
	s_Scripts.emplace(id, entry);
}

void ScriptSystem::ReloadAll() {
	s_Lua = sol::state {};
	s_Initialised = false;
	InitialiseState();

	for (auto& [id, entry] : s_Scripts) {
		Execute(entry.Source, id.ToString());
		entry.ReloadNecessary = false;
	}
}

void ScriptSystem::ReloadAllNecessary() {
	s_Lua = sol::state {};
	InitialiseState();

	for (auto& [id, entry] : s_Scripts) {
		if (!entry.ReloadNecessary)
			continue;

		Execute(entry.Source, id.ToString());
		entry.ReloadNecessary = false;
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
