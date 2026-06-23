#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>

#include "origo/core/UUID.h"

namespace Origo {

class NoctScriptRuntime;

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
	inline static std::unordered_map<UUID, ScriptEntry> s_scripts {};
	static std::unique_ptr<NoctScriptRuntime> s_noct_runtime;
	inline static bool s_initialised {};
};

}
