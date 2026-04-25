#pragma once

#include <unordered_map>
#include <vector>

#include "origo/core/RID.h"

#include "origo/serialization/ISerializer.h"

#include "origo/scripting/ScriptComponentInstance.h"

namespace Origo {

class ScriptComponentManager {
public:
	ScriptComponentManager();

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
	std::unordered_map<RID, std::vector<ScriptComponentInstance>> m_data {};
};

}
