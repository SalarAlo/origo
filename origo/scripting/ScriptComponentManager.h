#pragma once

#include <cstddef>
#include <unordered_map>
#include <vector>

#include "origo/core/RID.h"

#include "origo/serialization/ISerializer.h"

#include "origo/scripting/ScriptComponentInstance.h"

namespace Origo {

class ScriptComponentManager {
public:
	ScriptComponentManager();
	ScriptComponentManager(const ScriptComponentManager& other);
	ScriptComponentManager& operator=(const ScriptComponentManager& other);
	~ScriptComponentManager();

	void add(RID entity, ScriptComponentID type);

	ScriptComponentInstance* get(RID entity, ScriptComponentID type);

	const ScriptComponentInstance* get(RID entity, ScriptComponentID type) const;
	bool has(RID entity, ScriptComponentID type) const;
	void copy_components(RID source, RID target);
	void serialize_entity(RID entity, ISerializer& backend) const;
	void deserialize_entity(RID entity, ISerializer& backend);
	void remove(RID entity, ScriptComponentID type);
	bool remove_if_exists(RID entity, ScriptComponentID type);
	void remove_all_components(RID entity);
	void migrate_component(RID entity, ScriptComponentID);

private:
	void subscribe_to_registry_updates();
	void unsubscribe_from_registry_updates();

	std::unordered_map<RID, std::vector<ScriptComponentInstance>> m_data {};
	std::size_t m_script_component_updated_listener {};
	std::size_t m_script_component_removed_listener {};
	bool m_has_script_component_updated_listener {};
	bool m_has_script_component_removed_listener {};
};

}
