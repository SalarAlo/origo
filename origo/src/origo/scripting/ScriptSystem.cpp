#include "origo/scripting/ScriptSystem.h"
#include "origo/scripting/ScriptComponentRegistry.h"
#include "sol/error.hpp"
#include <stdexcept>

namespace Origo {

void ScriptSystem::InitialiseState() {
	if (s_Initialised)
		return;

	s_Initialised = true;
	s_Lua.open_libraries(sol::lib::base, sol::lib::io, sol::lib::math, sol::lib::table);
	sol::table componentTable = s_Lua.create_named_table("Component");

	componentTable.set_function(
	    "define",
	    [](const std::string& name,
	        sol::table fields,
	        sol::this_environment this_env) {
		    ORG_INFO("Component.define called from script with name {}", name);

		    if (!this_env.env)
			    throw std::runtime_error("Component.define called without a Lua environment");

		    sol::environment env = this_env.env.value();

		    sol::object uuidObj = env["__SCRIPT_UUID"];
		    if (!uuidObj.valid())
			    throw std::runtime_error(
			        "Component.define called outside import context");

		    std::string uuid = uuidObj.as<std::string>();

		    ScriptComponentRegistry::RegisterComponentFromLua(UUID::FromString(uuid), name, fields);
	    });
}

void ScriptSystem::Shutdown() {
	s_Lua = sol::state {};
	s_Initialised = false;
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
	Shutdown();
	InitialiseState();

	for (auto& [id, entry] : s_Scripts) {
		Execute(entry.Source, id);
		entry.ReloadNecessary = false;
	}
}

void ScriptSystem::ReloadAllNecessary() {
	Shutdown();
	InitialiseState();

	for (auto& [id, entry] : s_Scripts) {
		if (!entry.ReloadNecessary)
			continue;

		Execute(entry.Source, id);
		entry.ReloadNecessary = false;
	}
}

void ScriptSystem::Execute(const std::string& source, const UUID& id) {
	sol::environment env(s_Lua, sol::create, s_Lua.globals());
	env["__SCRIPT_UUID"] = id.ToString();

	sol::load_result load = s_Lua.load(source, id.ToString());
	if (!load.valid()) {
		sol::error err = load;
		throw std::runtime_error(err.what());
	}

	sol::protected_function fn = load;
	sol::set_environment(env, fn);

	sol::protected_function_result res = fn();
	if (!res.valid()) {
		sol::error err = res;
		throw std::runtime_error(err.what());
	}
}

}
