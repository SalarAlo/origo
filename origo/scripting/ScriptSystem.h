#pragma once

#include "origo/core/UUID.h"

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

namespace Origo {

class ScriptSystem {
public:
	struct ScriptEntry {
		std::string Source {};
		std::filesystem::path Path {};
		bool ReloadNecessary { true };
	};

public:
	static void initialise_state();
	static void shutdown();

	static void register_script(const UUID& id, const std::filesystem::path& path, const std::string& source);
	static void reload_all();
	static void reload_all_necessary();
	static void execute(const ScriptEntry& entry, const UUID& id);

private:
	inline static sol::state s_lua {};
	inline static std::unordered_map<UUID, ScriptEntry> s_scripts {};
	inline static bool s_initialised {};
};

}
