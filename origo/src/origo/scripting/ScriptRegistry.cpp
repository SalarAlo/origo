#include "origo/core/Logger.h"

#include "origo/scripting/ScriptComponentInstance.h"
#include "origo/scripting/ScriptComponentRegistry.h"

namespace Origo {

namespace {
	struct ScriptTypeInfo {
		VariantType Type;
		std::function<Variant(const sol::object&)> FromLua;
	};
}

static const std::unordered_map<std::string_view, ScriptTypeInfo> s_type_registry = {
	{ "int", { VariantType::Int, [](const sol::object& o) { return Variant(o.as<int>()); } } },
	{ "float", { VariantType::Float, [](const sol::object& o) { return Variant(o.as<float>()); } } },
	{ "bool", { VariantType::Bool, [](const sol::object& o) { return Variant(o.as<bool>()); } } },
	{ "string", { VariantType::String, [](const sol::object& o) { return Variant(o.as<std::string>()); } } },
};

ScriptComponentID ScriptComponentRegistry::register_or_update(ScriptComponentDescriptor descriptor) {
	if (s_descriptors.contains(descriptor.ID)) {
		ORG_CORE_TRACE("Registering already existent Component {}", descriptor.Name);
		auto& existing = s_descriptors[descriptor.ID];
		existing.Fields = descriptor.Fields;
		existing.Name = descriptor.Name;

		s_name_to_script_component_id.erase(existing.Name);
		s_name_to_script_component_id.emplace(descriptor.Name, descriptor.ID);

		on_script_component_updated().invoke(descriptor.ID);

		return descriptor.ID;
	}

	for (const auto& field : descriptor.Fields) {
		if (field.DefaultValue.get_type() != field.Type) {
			throw std::runtime_error("Field '" + field.Name + "' has mismatched default value type");
		}
	}

	ORG_CORE_TRACE("Registering Component {}", descriptor.Name);
	ScriptComponentID id = descriptor.ID;

	s_name_to_script_component_id.insert({ descriptor.Name, id });
	s_descriptors.insert({ id, std::move(descriptor) });

	return id;
}

ScriptComponentID ScriptComponentRegistry::register_component_from_lua(const UUID& uuid, const std::string& name, sol::table fields) {
	ScriptComponentDescriptor desc {};
	desc.Name = name;
	desc.ID = uuid;

	for (auto& kv : fields) {
		std::string field_name = kv.first.as<std::string>();
		sol::table field_def = kv.second.as<sol::table>();

		if (!field_def["type"].valid() || !field_def["default"].valid()) {
			throw std::runtime_error("Field '" + field_name + "' must have 'type' and 'default'");
		}

		std::string type_str = field_def["type"];
		sol::object def_val = field_def["default"];

		auto it = s_type_registry.find(type_str);

		if (it == s_type_registry.end()) {
			ORG_CORE_ERROR("Unknown script field type: " + type_str);
			throw std::runtime_error("Unknown script field type: " + type_str);
		}

		VariantType type = it->second.Type;
		Variant value = it->second.FromLua(def_val);

		UUID field_id = UUID::from_hash(
		    uuid.to_string() + "::" + field_name);

		ScriptFieldDescriptor field {
			field_id,
			field_name,
			type,
			value
		};

		desc.Fields.push_back(std::move(field));
	}

	std::sort(desc.Fields.begin(), desc.Fields.end(),
	    [](const ScriptFieldDescriptor& a,
	        const ScriptFieldDescriptor& b) {
		    return a.Name < b.Name;
	    });

	return ScriptComponentRegistry::register_or_update(std::move(desc));
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

bool ScriptComponentRegistry::exists(ScriptComponentID id) {
	return s_descriptors.contains(id);
}

Action<void, ScriptComponentID> ScriptComponentRegistry::on_script_component_updated() {
	static Action<void, ScriptComponentID> action {};
	return action;
}

}
