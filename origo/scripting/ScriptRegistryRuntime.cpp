#include <algorithm>
#include <stdexcept>
#include <unordered_set>

#include "origo/scripting/ScriptRegistry.h"

#include "origo/core/Logger.h"

namespace Origo {

namespace {
	FieldDescriptor create_field_descriptor(const FieldMetadata& metadata) {
		if (metadata.DefaultValue.get_type() != variant_type_from_reflected_type(metadata.Type.Kind))
			throw std::runtime_error("Field '" + metadata.Name + "' has mismatched default value type");

		return FieldDescriptor {
			.ID = metadata.ID,
			.Name = metadata.Name,
			.Type = metadata.Type,
			.DefaultValue = metadata.DefaultValue,
			.Annotations = metadata.Annotations,
			.Serialized = metadata.Serialized,
			.InspectorVisible = metadata.InspectorVisible
		};
	}

	void bind_lifecycle(ScriptLifecycleBindings& bindings, const FunctionMetadata& fn) {
		switch (fn.Lifecycle) {
		case LifecycleKind::OnStart:
			bindings.OnStart = fn.RuntimeHandle;
			bindings.OnStartBody = fn.BodySource;
			break;
		case LifecycleKind::OnUpdate:
			bindings.OnUpdate = fn.RuntimeHandle;
			bindings.OnUpdateBody = fn.BodySource;
			break;
		case LifecycleKind::OnFixedUpdate:
			bindings.OnFixedUpdate = fn.RuntimeHandle;
			bindings.OnFixedUpdateBody = fn.BodySource;
			break;
		case LifecycleKind::OnDestroy:
			bindings.OnDestroy = fn.RuntimeHandle;
			bindings.OnDestroyBody = fn.BodySource;
			break;
		case LifecycleKind::OnCollision:
			bindings.OnCollision = fn.RuntimeHandle;
			bindings.OnCollisionBody = fn.BodySource;
			break;
		case LifecycleKind::OnTriggerEnter:
			bindings.OnTriggerEnter = fn.RuntimeHandle;
			bindings.OnTriggerEnterBody = fn.BodySource;
			break;
		case LifecycleKind::OnTriggerExit:
			bindings.OnTriggerExit = fn.RuntimeHandle;
			bindings.OnTriggerExitBody = fn.BodySource;
			break;
		case LifecycleKind::None:
			break;
		}
	}

	bool has_any_lifecycle(const ScriptLifecycleBindings& bindings) {
		return bindings.OnStart != UUID::bad()
		    || bindings.OnUpdate != UUID::bad()
		    || bindings.OnFixedUpdate != UUID::bad()
		    || bindings.OnDestroy != UUID::bad()
		    || bindings.OnCollision != UUID::bad()
		    || bindings.OnTriggerEnter != UUID::bad()
		    || bindings.OnTriggerExit != UUID::bad();
	}
}

ScriptTypeID ScriptRegistry::register_or_update(ScriptDescriptor descriptor) {
	if (s_descriptors.contains(descriptor.ID)) {
		auto& existing = s_descriptors[descriptor.ID];
		const std::string old_name = existing.Name;

		existing = std::move(descriptor);

		s_name_to_script_id.erase(old_name);
		s_name_to_script_id.insert_or_assign(existing.Name, existing.ID);

		on_script_updated().invoke(existing.ID);
		return existing.ID;
	}

	ORG_CORE_TRACE("Registering behavior script {}", descriptor.Name);
	ScriptTypeID id = descriptor.ID;
	s_name_to_script_id.insert_or_assign(descriptor.Name, id);
	s_descriptors.insert({ id, std::move(descriptor) });
	return id;
}

ScriptDescriptor ScriptRegistry::create_descriptor(const ScriptMetadata& metadata) {
	ScriptDescriptor desc {};
	desc.ID = metadata.ID;
	desc.AssetID = metadata.AssetID;
	desc.Name = metadata.QualifiedName;
	desc.ComponentType = metadata.ComponentType;
	desc.Version = metadata.Version;
	desc.RuntimeType = metadata.RuntimeType;
	desc.Annotations = metadata.Annotations;
	desc.Fields.reserve(metadata.Fields.size());

	for (const FieldMetadata& field : metadata.Fields)
		desc.Fields.push_back(create_field_descriptor(field));

	for (const FunctionMetadata& fn : metadata.Functions) {
		if (fn.Lifecycle == LifecycleKind::None) {
			ORG_CORE_WARN("[Noct] Function '{}.{}' is not a recognized lifecycle function and will not be called automatically.",
			    metadata.QualifiedName,
			    fn.Name);
		}
		bind_lifecycle(desc.Lifecycle, fn);
	}

	if (!metadata.Functions.empty() && !has_any_lifecycle(desc.Lifecycle)) {
		ORG_CORE_WARN("[Noct] Script '{}' declares functions, but none are recognized lifecycle callbacks. Use names like on_start, on_update, or on_fixed_update.",
		    metadata.QualifiedName);
	}

	std::sort(desc.Fields.begin(), desc.Fields.end(), [](const FieldDescriptor& lhs, const FieldDescriptor& rhs) {
		return lhs.Name < rhs.Name;
	});

	return desc;
}

std::vector<ScriptTypeID> ScriptRegistry::replace_asset_scripts(const UUID& asset_id, std::vector<ScriptDescriptor> descriptors) {
	std::unordered_set<ScriptTypeID> incoming_ids;
	incoming_ids.reserve(descriptors.size());

	for (auto& descriptor : descriptors) {
		descriptor.AssetID = asset_id;
		incoming_ids.insert(descriptor.ID);
	}

	std::vector<ScriptTypeID> removed_ids;
	for (const auto& [id, descriptor] : s_descriptors) {
		if (descriptor.AssetID == asset_id && !incoming_ids.contains(id))
			removed_ids.push_back(id);
	}

	for (ScriptTypeID id : removed_ids) {
		auto descriptor_it = s_descriptors.find(id);
		if (descriptor_it == s_descriptors.end())
			continue;

		auto name_it = s_name_to_script_id.find(descriptor_it->second.Name);
		if (name_it != s_name_to_script_id.end() && name_it->second == id)
			s_name_to_script_id.erase(name_it);

		on_script_removed().invoke(id);
		s_descriptors.erase(descriptor_it);
	}

	std::vector<ScriptTypeID> registered_ids;
	registered_ids.reserve(descriptors.size());
	for (auto& descriptor : descriptors)
		registered_ids.push_back(register_or_update(std::move(descriptor)));

	return registered_ids;
}

const ScriptDescriptor& ScriptRegistry::get(ScriptTypeID id) {
	if (!s_descriptors.contains(id))
		throw std::runtime_error("Invalid ScriptTypeID");

	return s_descriptors[id];
}

std::optional<ScriptTypeID> ScriptRegistry::try_find_by_name(const std::string& name) {
	auto it = s_name_to_script_id.find(name);
	if (it == s_name_to_script_id.end())
		return std::nullopt;

	return it->second;
}

std::optional<ScriptTypeID> ScriptRegistry::try_find_by_asset_and_name(const UUID& asset_id, const std::string& name) {
	for (const auto& [id, descriptor] : s_descriptors) {
		if (descriptor.AssetID == asset_id && descriptor.Name == name)
			return id;
	}

	return std::nullopt;
}

std::vector<ScriptTypeID> ScriptRegistry::find_by_asset(const UUID& asset_id) {
	std::vector<ScriptTypeID> ids;
	for (const auto& [id, descriptor] : s_descriptors) {
		if (descriptor.AssetID == asset_id)
			ids.push_back(id);
	}
	return ids;
}

bool ScriptRegistry::exists(ScriptTypeID id) {
	return s_descriptors.contains(id);
}

Action<void, ScriptTypeID>& ScriptRegistry::on_script_updated() {
	static Action<void, ScriptTypeID> action {};
	return action;
}

Action<void, ScriptTypeID>& ScriptRegistry::on_script_removed() {
	static Action<void, ScriptTypeID> action {};
	return action;
}

}
