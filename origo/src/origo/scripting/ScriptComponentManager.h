#pragma once

#include <unordered_map>
#include <vector>

#include "origo/core/RID.h"
#include "origo/scripting/ScriptComponentInstance.h"

namespace Origo {

class ScriptComponentManager {
public:
	ScriptComponentManager();

	void Add(RID entity, ScriptComponentID type);

	ScriptComponentInstance* Get(RID entity, ScriptComponentID type);
	const ScriptComponentInstance* Get(RID entity, ScriptComponentID type) const;
	bool Has(RID entity, ScriptComponentID type) const;
	void Remove(RID entity, ScriptComponentID type);
	bool RemoveIfExists(RID entity, ScriptComponentID type);
	void MigrateComponent(RID entity, ScriptComponentID);

private:
	std::unordered_map<RID, std::vector<ScriptComponentInstance>> m_Data {};
};

}
