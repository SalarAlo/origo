#include "InspectorDrawRegistry.h"

namespace OrigoEditor {

InspectorEntry* InspectorDrawRegistry::get(std::type_index type) {
	auto it = s_entries.find(type);
	if (it == s_entries.end())
		return nullptr;

	return &it->second;
}

}
