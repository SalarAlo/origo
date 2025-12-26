#pragma once

#include <unordered_map>
#include <vector>

#include "origo/core/RID.h"
#include "origo/scripting/ScriptComponentInstance.h"

namespace Origo {

class ScriptComponentManager {
public:
	void Add(RID entity, ScriptComponentID type);

	ScriptComponentInstance* Get(RID entity, ScriptComponentID type);
	const ScriptComponentInstance* Get(RID entity, ScriptComponentID type) const;
	bool Has(RID entity, ScriptComponentID type) const;

private:
	std::unordered_map<RID, std::vector<ScriptComponentInstance>> m_Data;
};

}
