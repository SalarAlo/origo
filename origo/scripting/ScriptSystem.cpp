#include <stdexcept>

#include "origo/scripting/ScriptSystem.h"

#include "origo/scripting/ScriptComponentRegistry.h"

#include "sol/error.hpp"

namespace Origo {

void ScriptSystem::initialise_state() {
	if (s_initialised)
		return;

	s_initialised = true;
	s_lua.open_libraries(sol::lib::base, sol::lib::io, sol::lib::math, sol::lib::table);
	sol::table component_table = s_lua.create_named_table("Component");

	component_table.set_function(
	    "define",
	    [](const std::string& name,
	        sol::table fields,
	        sol::this_environment this_env) {
		    if (!this_env.env)
			    throw std::runtime_error("Component.define called without a Lua environment");

		    sol::environment env = this_env.env.value();

		    sol::object uuid_obj = env["__SCRIPT_UUID"];
		    if (!uuid_obj.valid())
			    throw std::runtime_error("Component.define called outside import context");

		    // this only ensures defined components in one file arent repeated
		    sol::table defined = env["__DEFINED_COMPONENTS"];
		    if (!defined.valid())
			    throw std::runtime_error("Internal error: __DEFINED_COMPONENTS missing");

		    if (defined[name].valid()) {
			    throw std::runtime_error(
			        "Component '" + name + "' is defined multiple times in the same script");
		    }

		    defined[name] = true;

		    std::string uuid = uuid_obj.as<std::string>();
		    ScriptComponentRegistry::register_component_from_lua(
		        UUID::from_string(uuid),
		        name,
		        fields);
	    });
}

void ScriptSystem::shutdown() {
	s_lua = sol::state {};
	s_initialised = false;
}

void ScriptSystem::register_script(const UUID& id, const std::filesystem::path& path, const std::string& source) {
	if (s_scripts.contains(id)) {
		auto& entry = s_scripts[id];
		entry.Path = path;
		entry.Source = source;
		entry.ReloadNecessary = true;
		return;
	}

	ScriptEntry entry {};
	entry.Source = source;
	entry.Path = path;
	s_scripts.emplace(id, entry);
}

void ScriptSystem::reload_all() {
	shutdown();
	initialise_state();

	for (auto& [id, entry] : s_scripts) {
		execute(entry, id);
		entry.ReloadNecessary = false;
	}
}

void ScriptSystem::reload_all_necessary() {
	shutdown();
	initialise_state();

	for (auto& [id, entry] : s_scripts) {
		if (!entry.ReloadNecessary)
			continue;

		execute(entry, id);
		entry.ReloadNecessary = false;
	}
}

void ScriptSystem::execute(const ScriptEntry& entry, const UUID& id) {
	sol::environment env(s_lua, sol::create, s_lua.globals());
	env["__SCRIPT_UUID"] = id.to_string();
	env["__DEFINED_COMPONENTS"] = sol::table::create(env.lua_state());

	sol::load_result load = s_lua.load(entry.Source, id.to_string());
	if (!load.valid()) {
		sol::error err = load;
		ORG_CORE_ERROR("{} failed to compile because {}", entry.Path.filename().c_str(), err.what());
		return;
	}

	sol::protected_function fn = load;
	sol::set_environment(env, fn);

	sol::protected_function_result res = fn();
	if (!res.valid()) {
		sol::error err = res;
		ORG_CORE_ERROR("{} failed to execute because {}", entry.Path.filename().c_str(), err.what());
	}
}

}
