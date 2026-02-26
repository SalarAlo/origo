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
	static ScriptComponentID register_or_update(ScriptComponentDescriptor descriptor);
	static ScriptComponentID register_component_from_lua(const UUID& uuid, const std::string& name, sol::table fields);

	static const ScriptComponentDescriptor& get(ScriptComponentID id);

	static std::optional<ScriptComponentID> try_find_by_name(const std::string& name);
	static bool exists(ScriptComponentID id);

	static const std::unordered_map<ScriptComponentID, ScriptComponentDescriptor>& get_all() { return s_descriptors; }
	static Action<void, ScriptComponentID> on_script_component_updated();

private:
	inline static std::unordered_map<ScriptComponentID, ScriptComponentDescriptor> s_descriptors {};
	inline static std::unordered_map<std::string, ScriptComponentID> s_name_to_script_component_id {};
};

}
