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
	static void InitialiseState();
	static void Shutdown();

	static void Register(const UUID& id, const std::filesystem::path& path, const std::string& source);
	static void ReloadAll();
	static void ReloadAllNecessary();
	static void Execute(const std::string& source, const UUID& id);

private:
	inline static sol::state s_Lua {};
	inline static std::unordered_map<UUID, ScriptEntry> s_Scripts {};
	inline static bool s_Initialised {};
};

}
