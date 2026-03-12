#include <algorithm>
#include <cassert>
#include <optional>

#include "origo/scripting/ScriptComponentManager.h"

#include "origo/core/Logger.h"

#include "origo/scripting/ScriptComponentInstance.h"
#include "origo/scripting/ScriptComponentRegistry.h"

namespace Origo {

namespace {

std::string variant_type_to_string(VariantType type) {
	switch (type) {
	case VariantType::Int:
		return "int";
	case VariantType::Bool:
		return "bool";
	case VariantType::Float:
		return "float";
	case VariantType::String:
		return "string";
	}

	return "unknown";
}

std::optional<VariantType> variant_type_from_string(const std::string& type) {
	if (type == "int")
		return VariantType::Int;
	if (type == "bool")
		return VariantType::Bool;
	if (type == "float")
		return VariantType::Float;
	if (type == "string")
		return VariantType::String;

	return std::nullopt;
}

void serialize_variant_value(const Variant& value, ISerializer& backend) {
	switch (value.get_type()) {
	case VariantType::Int: {
		int v {};
		if (value.TryGetAsInt(v))
			backend.write("value", v);
		return;
	}
	case VariantType::Bool: {
		bool v {};
		if (value.TryGetAsBool(v))
			backend.write("value", v);
		return;
	}
	case VariantType::Float: {
		float v {};
		if (value.TryGetAsFloat(v))
			backend.write("value", v);
		return;
	}
	case VariantType::String: {
		std::string v;
		if (value.TryGetAsString(v))
			backend.write("value", v);
		return;
	}
	}
}

std::optional<Variant> deserialize_variant_value(VariantType type, ISerializer& backend) {
	switch (type) {
	case VariantType::Int: {
		int value {};
		if (!backend.try_read("value", value))
			return std::nullopt;
		return Variant(value);
	}
	case VariantType::Bool: {
		bool value {};
		if (!backend.try_read("value", value))
			return std::nullopt;
		return Variant(value);
	}
	case VariantType::Float: {
		float value {};
		if (!backend.try_read("value", value))
			return std::nullopt;
		return Variant(value);
	}
	case VariantType::String: {
		std::string value;
		if (!backend.try_read("value", value))
			return std::nullopt;
		return Variant(value);
	}
	}

	return std::nullopt;
}

}

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

void ScriptComponentManager::copy_components(RID source, RID target) {
	auto it = m_data.find(source);
	if (it == m_data.end())
		return;

	m_data[target] = it->second;
}

void ScriptComponentManager::serialize_entity(RID entity, ISerializer& backend) const {
	backend.begin_array("script_components");

	auto it = m_data.find(entity);
	if (it != m_data.end()) {
		for (const auto& component : it->second) {
			backend.begin_array_object();
			backend.write("id", component.ID.to_string());

			if (ScriptComponentRegistry::exists(component.ID)) {
				backend.write("name", ScriptComponentRegistry::get(component.ID).Name);
			}

			backend.begin_array("fields");

			for (const auto& field : component.Fields) {
				backend.begin_array_object();
				backend.write("id", field.ID.to_string());
				backend.write("name", field.Name);
				backend.write("type", variant_type_to_string(field.Value.get_type()));
				serialize_variant_value(field.Value, backend);
				backend.end_array_object();
			}

			backend.end_array();
			backend.end_array_object();
		}
	}

	backend.end_array();
}

void ScriptComponentManager::deserialize_entity(RID entity, ISerializer& backend) {
	backend.begin_array("script_components");

	while (backend.try_begin_array_object_read()) {
		std::string component_id_str;
		std::string component_name;
		backend.try_read("id", component_id_str);
		backend.try_read("name", component_name);

		std::optional<ScriptComponentID> component_id;
		if (!component_id_str.empty()) {
			try {
				ScriptComponentID parsed_id = ScriptComponentID::from_string(component_id_str);
				if (ScriptComponentRegistry::exists(parsed_id))
					component_id = parsed_id;
			} catch (const std::exception&) {
			}
		}

		if (!component_id.has_value() && !component_name.empty()) {
			component_id = ScriptComponentRegistry::try_find_by_name(component_name);
		}

		if (!component_id.has_value()) {
			ORG_WARN("[Scene] Unknown script component '{}' ({})", component_name, component_id_str);
			backend.end_array_object();
			continue;
		}

		add(entity, *component_id);
		auto* instance = get(entity, *component_id);

		backend.begin_array("fields");

		while (backend.try_begin_array_object_read()) {
			std::string field_id_str;
			std::string field_name;
			std::string field_type_str;

			backend.try_read("id", field_id_str);
			backend.try_read("name", field_name);
			backend.try_read("type", field_type_str);

			const auto field_type = variant_type_from_string(field_type_str);
			if (!field_type.has_value()) {
				backend.end_array_object();
				continue;
			}

			const auto value = deserialize_variant_value(*field_type, backend);
			if (!value.has_value()) {
				backend.end_array_object();
				continue;
			}

			if (instance) {
				auto field_it = std::find_if(
				    instance->Fields.begin(),
				    instance->Fields.end(),
				    [&](const ScriptComponentFieldInstance& field) {
					    if (!field_id_str.empty()) {
						    try {
							    return field.ID == ScriptComponentFieldID::from_string(field_id_str);
						    } catch (const std::exception&) {
						    }
					    }

					    return field.Name == field_name;
				    });

				if (field_it != instance->Fields.end() && field_it->Value.get_type() == *field_type) {
					field_it->Value = *value;
				}
			}

			backend.end_array_object();
		}

		backend.end_array();
		backend.end_array_object();
	}

	backend.end_array();
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

	const bool removed = components.size() != old_size;

	if (components.empty()) {
		m_data.erase(it);
	}

	return removed;
}

void ScriptComponentManager::remove_all_components(RID entity) {
	m_data.erase(entity);
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
