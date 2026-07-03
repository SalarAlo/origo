#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "origo/core/Action.h"
#include "origo/scripting/ScriptDescriptor.h"

namespace Origo {

class ScriptRegistry {
public:
	static ScriptTypeID register_or_update(ScriptDescriptor descriptor);
	static ScriptDescriptor create_descriptor(const ScriptMetadata& metadata);
	static std::vector<ScriptTypeID> replace_asset_scripts(const UUID& asset_id, std::vector<ScriptDescriptor> descriptors);

	static const ScriptDescriptor& get(ScriptTypeID id);
	static std::optional<ScriptTypeID> try_find_by_name(const std::string& name);
	static std::optional<ScriptTypeID> try_find_by_asset_and_name(const UUID& asset_id, const std::string& name);
	static std::vector<ScriptTypeID> find_by_asset(const UUID& asset_id);
	static bool exists(ScriptTypeID id);

	static const std::unordered_map<ScriptTypeID, ScriptDescriptor>& get_all() { return s_descriptors; }
	static Action<void, ScriptTypeID>& on_script_updated();
	static Action<void, ScriptTypeID>& on_script_removed();

private:
	inline static std::unordered_map<ScriptTypeID, ScriptDescriptor> s_descriptors {};
	inline static std::unordered_map<std::string, ScriptTypeID> s_name_to_script_id {};
};

}
