#pragma once

#include "origo/core/UUID.h"

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

namespace Origo {

class ScriptSystem {
public:
	struct ScriptEntry {
		std::string Source {};
	};

public:
	static void InitialiseState();
	static void Shutdown();

	static void Register(const UUID& id, std::string source);
	static void ReloadAll();
	static void Execute(const std::string& source, const std::string& debugName);

private:
	inline static sol::state s_Lua {};
	inline static std::vector<std::pair<UUID, ScriptEntry>> s_Scripts {};
};

}
