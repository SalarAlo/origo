#include "InspectorDrawRegistry.h"

namespace OrigoEditor {

InspectorEntry* InspectorDrawRegistry::Get(std::type_index type) {
	auto it = s_Entries.find(type);
	if (it == s_Entries.end())
		return nullptr;

	return &it->second;
}

}
