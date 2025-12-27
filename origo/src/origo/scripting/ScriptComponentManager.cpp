#include "origo/scripting/ScriptComponentManager.h"
#include "origo/scripting/ScriptComponentRegistry.h"
#include <cassert>

namespace Origo {

ScriptComponentManager::ScriptComponentManager() {
	ScriptComponentRegistry::OnScriptComponentUpdated().AddListener(
	    [&](ScriptComponentID updatedComponent) {
		    std::vector<RID> affected;

		    for (const auto& [e, components] : m_Data) {
			    for (const auto& c : components) {
				    if (c.ID == updatedComponent) {
					    affected.push_back(e);
					    break;
				    }
			    }
		    }

		    for (RID e : affected) {
			    MigrateComponent(e, updatedComponent);
		    }
	    });
}

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
		instance.FieldNames.push_back(field.Name);
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

void ScriptComponentManager::Remove(RID entity, ScriptComponentID type) {
	auto it = m_Data.find(entity);
	if (it == m_Data.end())
		return;

	auto& components = it->second;

	components.erase(
	    std::remove_if(
	        components.begin(),
	        components.end(),
	        [type](const ScriptComponentInstance& c) {
		        return c.ID == type;
	        }),
	    components.end());

	if (components.empty()) {
		m_Data.erase(it);
	}
}

bool ScriptComponentManager::RemoveIfExists(RID entity, ScriptComponentID type) {
	auto it = m_Data.find(entity);
	if (it == m_Data.end())
		return false;

	auto& components = it->second;
	const size_t oldSize = components.size();

	components.erase(
	    std::remove_if(
	        components.begin(),
	        components.end(),
	        [type](const ScriptComponentInstance& c) {
		        return c.ID == type;
	        }),
	    components.end());

	if (components.empty()) {
		m_Data.erase(it);
	}

	return components.size() != oldSize;
}

void ScriptComponentManager::MigrateComponent(RID entity, ScriptComponentID type) {
	auto* instance = Get(entity, type);

	if (!instance)
		return;

	const auto& desc = ScriptComponentRegistry::Get(type);

	std::vector<Variant> newValues;
	std::vector<std::string> newNames;

	newValues.reserve(desc.Fields.size());
	newNames.reserve(desc.Fields.size());

	for (const auto& field : desc.Fields) {
		bool found = false;

		for (size_t i = 0; i < instance->FieldNames.size(); ++i) {
			if (instance->FieldNames[i] == field.Name && instance->Values[i].GetType() == field.Type) {

				newValues.push_back(instance->Values[i]);
				newNames.push_back(field.Name);
				found = true;
				break;
			}
		}

		if (!found) {
			newValues.push_back(field.DefaultValue);
			newNames.push_back(field.Name);
		}
	}

	instance->Values = std::move(newValues);
	instance->FieldNames = std::move(newNames);
}

}
