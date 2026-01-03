#include "origo/scripting/ScriptComponentManager.h"
#include "origo/scripting/ScriptComponentInstance.h"
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
	instance.Fields.reserve(desc.Fields.size());

	for (const auto& field : desc.Fields) {
		instance.Fields.emplace_back(field.ID, field.Name, field.DefaultValue);
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

	std::vector<ScriptComponentFieldInstance> newFields;

	newFields.reserve(desc.Fields.size());

	for (const auto& newField : desc.Fields) {
		auto it = std::find_if(
		    instance->Fields.begin(),
		    instance->Fields.end(),
		    [&](const ScriptComponentFieldInstance& oldField) {
			    return oldField.ID == newField.ID;
		    });

		if (it != instance->Fields.end()) {
			if (it->Value.GetType() == newField.Type) {
				newFields.emplace_back(newField.ID, newField.Name, it->Value);
			} else {
				newFields.emplace_back(newField.ID, newField.Name, newField.DefaultValue);
			}
		} else {
			newFields.emplace_back(
			    newField.ID,
			    newField.Name,
			    newField.DefaultValue);
		}
	}

	instance->Fields = std::move(newFields);
}

}
