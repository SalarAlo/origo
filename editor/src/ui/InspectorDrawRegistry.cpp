#include "InspectorDrawRegistry.h"

namespace OrigoEditor {

InspectorEntry* InspectorDrawRegistry::get_inspector_entry(std::type_index type) {
	auto it = m_entries.find(type);
	if (it == m_entries.end())
		return nullptr;

	return &it->second;
}

}
