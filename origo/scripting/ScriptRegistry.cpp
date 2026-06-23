#include <algorithm>
#include <stdexcept>
#include <unordered_set>

#include "origo/core/Logger.h"

#include "origo/scripting/ScriptComponentInstance.h"
#include "origo/scripting/ScriptComponentRegistry.h"

namespace Origo {

ScriptComponentID ScriptComponentRegistry::register_or_update(ScriptComponentDescriptor descriptor) {
	for (const auto& field : descriptor.Fields) {
		if (field.DefaultValue.get_type() != field.Type) {
			throw std::runtime_error("Field '" + field.Name + "' has mismatched default value type");
		}
	}

	if (s_descriptors.contains(descriptor.ID)) {
		ORG_CORE_TRACE("Registering already existent Component {}", descriptor.Name);
		auto& existing = s_descriptors[descriptor.ID];
		const std::string old_name = existing.Name;

		existing.Fields = descriptor.Fields;
		existing.Name = descriptor.Name;
		existing.ScriptID = descriptor.ScriptID;

		s_name_to_script_component_id.erase(old_name);
		s_name_to_script_component_id.insert_or_assign(descriptor.Name, descriptor.ID);

		on_script_component_updated().invoke(descriptor.ID);

		return descriptor.ID;
	}

	ORG_CORE_TRACE("Registering Component {}", descriptor.Name);
	ScriptComponentID id = descriptor.ID;

	s_name_to_script_component_id.insert_or_assign(descriptor.Name, id);
	s_descriptors.insert({ id, std::move(descriptor) });

	return id;
}

ScriptComponentDescriptor ScriptComponentRegistry::create_descriptor(const UUID& script_id, const std::string& name, std::vector<ScriptFieldDeclaration> fields) {
	ScriptComponentDescriptor desc {};
	desc.Name = name;
	desc.ScriptID = script_id;
	desc.ID = UUID::from_hash(script_id.to_string() + "::" + name);

	for (auto& declaration : fields) {
		UUID field_id = UUID::from_hash(
		    desc.ID.to_string() + "::" + declaration.Name);

		ScriptFieldDescriptor field {
			field_id,
			declaration.Name,
			declaration.Type,
			declaration.DefaultValue
		};

		desc.Fields.push_back(std::move(field));
	}

	std::sort(desc.Fields.begin(), desc.Fields.end(),
	    [](const ScriptFieldDescriptor& a,
	        const ScriptFieldDescriptor& b) {
		    return a.Name < b.Name;
	    });

	return desc;
}

ScriptComponentID ScriptComponentRegistry::register_component(const UUID& script_id, const std::string& name, std::vector<ScriptFieldDeclaration> fields) {
	return ScriptComponentRegistry::register_or_update(
	    ScriptComponentRegistry::create_descriptor(script_id, name, std::move(fields)));
}

std::vector<ScriptComponentID> ScriptComponentRegistry::replace_script_components(const UUID& script_id, std::vector<ScriptComponentDescriptor> descriptors) {
	std::unordered_set<ScriptComponentID> incoming_ids;
	incoming_ids.reserve(descriptors.size());

	for (auto& descriptor : descriptors) {
		descriptor.ScriptID = script_id;
		incoming_ids.insert(descriptor.ID);
	}

	std::vector<ScriptComponentID> removed_ids;
	for (const auto& [id, descriptor] : s_descriptors) {
		if (descriptor.ScriptID == script_id && !incoming_ids.contains(id)) {
			removed_ids.push_back(id);
		}
	}

	for (const auto& id : removed_ids) {
		const auto descriptor_it = s_descriptors.find(id);
		if (descriptor_it == s_descriptors.end())
			continue;

		auto name_it = s_name_to_script_component_id.find(descriptor_it->second.Name);
		if (name_it != s_name_to_script_component_id.end() && name_it->second == id)
			s_name_to_script_component_id.erase(name_it);

		on_script_component_removed().invoke(id);
		s_descriptors.erase(descriptor_it);
	}

	std::vector<ScriptComponentID> registered_ids;
	registered_ids.reserve(descriptors.size());
	for (auto& descriptor : descriptors) {
		registered_ids.push_back(register_or_update(std::move(descriptor)));
	}

	return registered_ids;
}

const ScriptComponentDescriptor& ScriptComponentRegistry::get(ScriptComponentID id) {
	if (!s_descriptors.contains(id)) {
		throw std::runtime_error("Invalid ScriptComponentID");
	}

	return s_descriptors[id];
}

std::optional<ScriptComponentID> ScriptComponentRegistry::try_find_by_name(const std::string& name) {
	auto it = s_name_to_script_component_id.find(name);

	if (it == s_name_to_script_component_id.end()) {
		return std::nullopt;
	}

	return it->second;
}

std::optional<ScriptComponentID> ScriptComponentRegistry::try_find_by_script_and_name(const UUID& script_id, const std::string& name) {
	for (const auto& [id, descriptor] : s_descriptors) {
		if (descriptor.ScriptID == script_id && descriptor.Name == name) {
			return id;
		}
	}

	return std::nullopt;
}

bool ScriptComponentRegistry::exists(ScriptComponentID id) {
	return s_descriptors.contains(id);
}

Action<void, ScriptComponentID>& ScriptComponentRegistry::on_script_component_updated() {
	static Action<void, ScriptComponentID> action {};
	return action;
}

Action<void, ScriptComponentID>& ScriptComponentRegistry::on_script_component_removed() {
	static Action<void, ScriptComponentID> action {};
	return action;
}

}
