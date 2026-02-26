#include "origo/scripting/ScriptComponentManager.h"
#include "origo/scripting/ScriptComponentInstance.h"
#include "origo/scripting/ScriptComponentRegistry.h"
#include <cassert>

namespace Origo {

ScriptComponentManager::ScriptComponentManager() {
	ScriptComponentRegistry::on_script_component_updated().add_listener(
	    [&](ScriptComponentID updatedComponent) {
		    std::vector<RID> affected;

		    for (const auto& [e, components] : m_data) {
			    for (const auto& c : components) {
				    if (c.ID == updatedComponent) {
					    affected.push_back(e);
					    break;
				    }
			    }
		    }

		    for (RID e : affected) {
			    migrate_component(e, updatedComponent);
		    }
	    });
}

void ScriptComponentManager::add(RID entity, ScriptComponentID type) {
	auto& components = m_data[entity];

	for (const auto& c : components) {
		if (c.ID == type) {
			return;
		}
	}

	const auto& desc = ScriptComponentRegistry::get(type);

	ScriptComponentInstance instance;
	instance.ID = type;
	instance.Fields.reserve(desc.Fields.size());

	for (const auto& field : desc.Fields) {
		instance.Fields.emplace_back(field.ID, field.Name, field.DefaultValue);
	}

	components.push_back(std::move(instance));
}

ScriptComponentInstance* ScriptComponentManager::get(RID entity, ScriptComponentID type) {
	auto it = m_data.find(entity);
	if (it == m_data.end())
		return nullptr;

	for (auto& c : it->second) {
		if (c.ID == type)
			return &c;
	}

	return nullptr;
}

const ScriptComponentInstance* ScriptComponentManager::get(RID entity, ScriptComponentID type) const {
	auto it = m_data.find(entity);
	if (it == m_data.end())
		return nullptr;

	for (auto& c : it->second) {
		if (c.ID == type)
			return &c;
	}

	return nullptr;
}

bool ScriptComponentManager::has(RID entity, ScriptComponentID type) const {
	auto it = m_data.find(entity);
	if (it == m_data.end())
		return false;

	for (const auto& c : it->second) {
		if (c.ID == type)
			return true;
	}

	return false;
}

void ScriptComponentManager::remove(RID entity, ScriptComponentID type) {
	auto it = m_data.find(entity);
	if (it == m_data.end())
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
		m_data.erase(it);
	}
}

bool ScriptComponentManager::remove_if_exists(RID entity, ScriptComponentID type) {
	auto it = m_data.find(entity);
	if (it == m_data.end())
		return false;

	auto& components = it->second;
	const size_t old_size = components.size();

	components.erase(
	    std::remove_if(
	        components.begin(),
	        components.end(),
	        [type](const ScriptComponentInstance& c) {
		        return c.ID == type;
	        }),
	    components.end());

	if (components.empty()) {
		m_data.erase(it);
	}

	return components.size() != old_size;
}

void ScriptComponentManager::migrate_component(RID entity, ScriptComponentID type) {
	auto* instance = get(entity, type);

	if (!instance)
		return;

	const auto& desc = ScriptComponentRegistry::get(type);

	std::vector<ScriptComponentFieldInstance> new_fields;

	new_fields.reserve(desc.Fields.size());

	for (const auto& new_field : desc.Fields) {
		auto it = std::find_if(
		    instance->Fields.begin(),
		    instance->Fields.end(),
		    [&](const ScriptComponentFieldInstance& oldField) {
			    return oldField.ID == new_field.ID;
		    });

		if (it != instance->Fields.end()) {
			if (it->Value.get_type() == new_field.Type) {
				new_fields.emplace_back(new_field.ID, new_field.Name, it->Value);
			} else {
				new_fields.emplace_back(new_field.ID, new_field.Name, new_field.DefaultValue);
			}
		} else {
			new_fields.emplace_back(
			    new_field.ID,
			    new_field.Name,
			    new_field.DefaultValue);
		}
	}

	instance->Fields = std::move(new_fields);
}

}
