#include <algorithm>
#include <stdexcept>
#include <unordered_set>

#include "origo/scripting/ComponentRegistry.h"

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
}

ComponentTypeID ComponentRegistry::register_or_update(ComponentDescriptor descriptor) {
	if (s_descriptors.contains(descriptor.ID)) {
		auto& existing = s_descriptors[descriptor.ID];
		const std::string old_name = existing.Name;

		existing = std::move(descriptor);

		s_name_to_component_id.erase(old_name);
		s_name_to_component_id.insert_or_assign(existing.Name, existing.ID);

		on_component_updated().invoke(existing.ID);
		return existing.ID;
	}

	ORG_CORE_TRACE("Registering reflected component {}", descriptor.Name);
	ComponentTypeID id = descriptor.ID;
	s_name_to_component_id.insert_or_assign(descriptor.Name, id);
	s_descriptors.insert({ id, std::move(descriptor) });
	return id;
}

ComponentDescriptor ComponentRegistry::create_descriptor(const ComponentMetadata& metadata) {
	ComponentDescriptor desc {};
	desc.ID = metadata.ID;
	desc.AssetID = metadata.AssetID;
	desc.Name = metadata.QualifiedName;
	desc.Version = metadata.Version;
	desc.Annotations = metadata.Annotations;
	desc.Fields.reserve(metadata.Fields.size());

	for (const FieldMetadata& field : metadata.Fields)
		desc.Fields.push_back(create_field_descriptor(field));

	std::sort(desc.Fields.begin(), desc.Fields.end(), [](const FieldDescriptor& lhs, const FieldDescriptor& rhs) {
		return lhs.Name < rhs.Name;
	});

	return desc;
}

std::vector<ComponentTypeID> ComponentRegistry::replace_asset_components(const UUID& asset_id, std::vector<ComponentDescriptor> descriptors) {
	std::unordered_set<ComponentTypeID> incoming_ids;
	incoming_ids.reserve(descriptors.size());

	for (auto& descriptor : descriptors) {
		descriptor.AssetID = asset_id;
		incoming_ids.insert(descriptor.ID);
	}

	std::vector<ComponentTypeID> removed_ids;
	for (const auto& [id, descriptor] : s_descriptors) {
		if (descriptor.AssetID == asset_id && !incoming_ids.contains(id))
			removed_ids.push_back(id);
	}

	for (ComponentTypeID id : removed_ids) {
		auto descriptor_it = s_descriptors.find(id);
		if (descriptor_it == s_descriptors.end())
			continue;

		auto name_it = s_name_to_component_id.find(descriptor_it->second.Name);
		if (name_it != s_name_to_component_id.end() && name_it->second == id)
			s_name_to_component_id.erase(name_it);

		on_component_removed().invoke(id);
		s_descriptors.erase(descriptor_it);
	}

	std::vector<ComponentTypeID> registered_ids;
	registered_ids.reserve(descriptors.size());
	for (auto& descriptor : descriptors)
		registered_ids.push_back(register_or_update(std::move(descriptor)));

	return registered_ids;
}

const ComponentDescriptor& ComponentRegistry::get(ComponentTypeID id) {
	if (!s_descriptors.contains(id))
		throw std::runtime_error("Invalid ComponentTypeID");

	return s_descriptors[id];
}

std::optional<ComponentTypeID> ComponentRegistry::try_find_by_name(const std::string& name) {
	auto it = s_name_to_component_id.find(name);
	if (it == s_name_to_component_id.end())
		return std::nullopt;

	return it->second;
}

std::optional<ComponentTypeID> ComponentRegistry::try_find_by_asset_and_name(const UUID& asset_id, const std::string& name) {
	for (const auto& [id, descriptor] : s_descriptors) {
		if (descriptor.AssetID == asset_id && descriptor.Name == name)
			return id;
	}

	return std::nullopt;
}

std::vector<ComponentTypeID> ComponentRegistry::find_by_asset(const UUID& asset_id) {
	std::vector<ComponentTypeID> ids;
	for (const auto& [id, descriptor] : s_descriptors) {
		if (descriptor.AssetID == asset_id)
			ids.push_back(id);
	}
	return ids;
}

bool ComponentRegistry::exists(ComponentTypeID id) {
	return s_descriptors.contains(id);
}

Action<void, ComponentTypeID>& ComponentRegistry::on_component_updated() {
	static Action<void, ComponentTypeID> action {};
	return action;
}

Action<void, ComponentTypeID>& ComponentRegistry::on_component_removed() {
	static Action<void, ComponentTypeID> action {};
	return action;
}

}
