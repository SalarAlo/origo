#include "origo/scripting/ScriptComponentRegistry.h"

#include "origo/core/Logger.h"
#include "origo/scripting/ScriptComponentInstance.h"

namespace Origo {

namespace {
	struct ScriptTypeInfo {
		VariantType Type;
		std::function<Variant(const sol::object&)> FromLua;
	};
}

static const std::unordered_map<std::string_view, ScriptTypeInfo> s_TypeRegistry = {
	{ "int", { VariantType::Int, [](const sol::object& o) { return Variant(o.as<int>()); } } },
	{ "float", { VariantType::Float, [](const sol::object& o) { return Variant(o.as<float>()); } } },
	{ "bool", { VariantType::Bool, [](const sol::object& o) { return Variant(o.as<bool>()); } } },
	{ "string", { VariantType::String, [](const sol::object& o) { return Variant(o.as<std::string>()); } } },
};

ScriptComponentID ScriptComponentRegistry::RegisterOrUpdate(ScriptComponentDescriptor descriptor) {
	if (s_Descriptors.contains(descriptor.ID)) {
		ORG_CORE_TRACE("Registering already existent Component {}", descriptor.Name);
		auto& existing = s_Descriptors[descriptor.ID];
		existing.Fields = descriptor.Fields;
		existing.Name = descriptor.Name;

		s_NameToScriptComponentID.erase(existing.Name);
		s_NameToScriptComponentID[descriptor.Name] = descriptor.ID;
		OnScriptComponentUpdated().Invoke(descriptor.ID);
		return descriptor.ID;
	}

	for (const auto& field : descriptor.Fields) {
		if (field.DefaultValue.GetType() != field.Type) {
			throw std::runtime_error("Field '" + field.Name + "' has mismatched default value type");
		}
	}

	ORG_CORE_TRACE("Registering Component {}", descriptor.Name);
	ScriptComponentID id = descriptor.ID;

	s_NameToScriptComponentID.insert({ descriptor.Name, id });
	s_Descriptors.insert({ id, std::move(descriptor) });

	return id;
}

ScriptComponentID ScriptComponentRegistry::RegisterComponentFromLua(const UUID& uuid, const std::string& name, sol::table fields) {
	ScriptComponentDescriptor desc;
	desc.Name = name;
	desc.ID = uuid;

	for (auto& kv : fields) {
		std::string fieldName = kv.first.as<std::string>();
		sol::table fieldDef = kv.second.as<sol::table>();

		if (!fieldDef["type"].valid() || !fieldDef["default"].valid()) {
			throw std::runtime_error("Field '" + fieldName + "' must have 'type' and 'default'");
		}

		std::string typeStr = fieldDef["type"];
		sol::object defVal = fieldDef["default"];

		auto it = s_TypeRegistry.find(typeStr);

		if (it == s_TypeRegistry.end()) {
			ORG_CORE_ERROR("Unknown script field type: " + typeStr);
			throw std::runtime_error("Unknown script field type: " + typeStr);
		}

		VariantType type = it->second.Type;
		Variant value = it->second.FromLua(defVal);

		ScriptFieldDescriptor field { fieldName, type, value };

		desc.Fields.push_back(std::move(field));
	}

	return ScriptComponentRegistry::RegisterOrUpdate(std::move(desc));
}

const ScriptComponentDescriptor& ScriptComponentRegistry::Get(ScriptComponentID id) {
	if (!s_Descriptors.contains(id)) {
		throw std::runtime_error("Invalid ScriptComponentID");
	}

	return s_Descriptors[id];
}

std::optional<ScriptComponentID> ScriptComponentRegistry::TryFindByName(const std::string& name) {
	auto it = s_NameToScriptComponentID.find(name);

	if (it == s_NameToScriptComponentID.end()) {
		return std::nullopt;
	}

	return it->second;
}
Action<void, ScriptComponentID> ScriptComponentRegistry::OnScriptComponentUpdated() {
	static Action<void, ScriptComponentID> action {};
	return action;
}

}
