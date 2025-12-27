#include "origo/scripting/ScriptComponentInstance.h"
#include "origo/scripting/ScriptComponentManager.h"
#include "origo/scripting/ScriptComponentRegistry.h"

namespace Origo {

static VariantType ParseType(const std::string& type) {
	if (type == "int")
		return VariantType::Int;
	if (type == "float")
		return VariantType::Float;
	if (type == "bool")
		return VariantType::Bool;
	if (type == "string")
		return VariantType::String;

	throw std::runtime_error("Unknown field type: " + type);
}

static Variant MakeVariant(
    VariantType type,
    const sol::object& obj) {
	switch (type) {
	case VariantType::Int:
		return Variant(obj.as<int>());
	case VariantType::Float:
		return Variant(obj.as<float>());
	case VariantType::Bool:
		return Variant(obj.as<bool>());
	case VariantType::String:
		return Variant(obj.as<std::string>());
	}

	throw std::runtime_error("Invalid VariantType");
}

ScriptComponentID ScriptComponentRegistry::RegisterOrUpdate(ScriptComponentDescriptor descriptor) {
	auto& descriptors = Descriptors();
	auto& nameMap = NameToID();

	if (nameMap.contains(descriptor.Name)) {
		auto id = nameMap[descriptor.Name];
		descriptors[id].Fields = descriptor.Fields;
		OnScriptComponentUpdated().Invoke(id);
		return id;
	}

	for (const auto& field : descriptor.Fields) {
		if (field.DefaultValue.GetType() != field.Type) {
			throw std::runtime_error("Field '" + field.Name + "' has mismatched default value type");
		}
	}

	ScriptComponentID id = static_cast<ScriptComponentID>(descriptors.size());

	descriptors.push_back(std::move(descriptor));
	nameMap[descriptors.back().Name] = id;

	return id;
}

ScriptComponentID ScriptComponentRegistry::RegisterComponentFromLua(const std::string& name, sol::table fields) {
	ScriptComponentDescriptor desc;
	desc.Name = name;

	for (auto& kv : fields) {
		std::string fieldName = kv.first.as<std::string>();
		sol::table fieldDef = kv.second.as<sol::table>();

		if (!fieldDef["type"].valid() || !fieldDef["default"].valid()) {
			throw std::runtime_error(
			    "Field '" + fieldName + "' must have 'type' and 'default'");
		}

		std::string typeStr = fieldDef["type"];
		sol::object defVal = fieldDef["default"];

		VariantType type = ParseType(typeStr);

		ScriptFieldDescriptor field { fieldName, type, MakeVariant(type, defVal) };

		desc.Fields.push_back(std::move(field));
	}

	return ScriptComponentRegistry::RegisterOrUpdate(std::move(desc));
}

const ScriptComponentDescriptor& ScriptComponentRegistry::Get(ScriptComponentID id) {
	auto& descriptors = Descriptors();

	if (id < 0 || id >= static_cast<int>(descriptors.size())) {
		throw std::runtime_error("Invalid ScriptComponentID");
	}

	return descriptors[id];
}

std::optional<ScriptComponentID> ScriptComponentRegistry::TryFindByName(const std::string& name) {
	auto& map = NameToID();
	auto it = map.find(name);

	if (it == map.end()) {
		return std::nullopt;
	}

	return it->second;
}
Action<void, ScriptComponentID> ScriptComponentRegistry::OnScriptComponentUpdated() {
	static Action<void, ScriptComponentID> action {};
	return action;
}

std::vector<ScriptComponentDescriptor>& ScriptComponentRegistry::Descriptors() {
	static std::vector<ScriptComponentDescriptor> s_Descriptors;
	return s_Descriptors;
}

std::unordered_map<std::string, ScriptComponentID>& ScriptComponentRegistry::NameToID() {
	static std::unordered_map<std::string, ScriptComponentID> s_Map;
	return s_Map;
}

}
