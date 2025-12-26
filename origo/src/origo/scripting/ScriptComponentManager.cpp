#include "origo/scripting/ScriptComponentManager.h"
#include "origo/scripting/ScriptComponentRegistry.h"
#include <cassert>

namespace Origo {

void ScriptComponentManager::Add(RID entity, ScriptComponentID type) {
	auto& components = m_Data[entity];

	for (const auto& c : components) {
		if (c.ID == type) {
			return;
		}
	}

	const auto& desc = ScriptComponentRegistry::Get(type);

	ScriptComponentInstance instance;
	instance.ID = type;
	instance.Values.reserve(desc.Fields.size());

	for (const auto& field : desc.Fields) {
		assert(field.DefaultValue.GetType() == field.Type);
		instance.Values.push_back(field.DefaultValue);
	}

	components.push_back(std::move(instance));
}

ScriptComponentInstance* ScriptComponentManager::Get(RID entity, ScriptComponentID type) {
	auto it = m_Data.find(entity);
	if (it == m_Data.end())
		return nullptr;

	for (auto& c : it->second) {
		if (c.ID == type)
			return &c;
	}

	return nullptr;
}

const ScriptComponentInstance* ScriptComponentManager::Get(RID entity, ScriptComponentID type) const {
	auto it = m_Data.find(entity);
	if (it == m_Data.end())
		return nullptr;

	for (auto& c : it->second) {
		if (c.ID == type)
			return &c;
	}

	return nullptr;
}

bool ScriptComponentManager::Has(RID entity, ScriptComponentID type) const {
	auto it = m_Data.find(entity);
	if (it == m_Data.end())
		return false;

	for (const auto& c : it->second) {
		if (c.ID == type)
			return true;
	}

	return false;
}

}
