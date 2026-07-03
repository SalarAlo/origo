#include <stdexcept>
#include <unordered_set>

#include "origo/scripting/StructRegistry.h"

namespace Origo {

void StructRegistry::replace_asset_structs(const UUID& asset_id, const std::vector<StructMetadata>& metadata) {
	std::unordered_set<TypeID> incoming;
	for (const auto& item : metadata)
		incoming.insert(item.ID);

	std::vector<TypeID> removed;
	for (const auto& [id, desc] : s_descriptors) {
		if (desc.AssetID == asset_id && !incoming.contains(id))
			removed.push_back(id);
	}

	for (TypeID id : removed) {
		on_struct_removed().invoke(id);
		s_descriptors.erase(id);
	}

	for (const auto& item : metadata) {
		StructDescriptor desc {};
		desc.ID = item.ID;
		desc.AssetID = item.AssetID;
		desc.Name = item.QualifiedName;
		desc.Version = item.Version;
		desc.Annotations = item.Annotations;
		for (const auto& field : item.Fields) {
			desc.Fields.push_back(FieldDescriptor {
				.ID = field.ID,
				.Name = field.Name,
				.Type = field.Type,
				.DefaultValue = field.DefaultValue,
				.Annotations = field.Annotations,
				.Serialized = field.Serialized,
				.InspectorVisible = field.InspectorVisible });
		}
		s_descriptors.insert_or_assign(desc.ID, std::move(desc));
		on_struct_updated().invoke(item.ID);
	}
}

bool StructRegistry::exists(TypeID id) {
	return s_descriptors.contains(id);
}

const StructDescriptor& StructRegistry::get(TypeID id) {
	if (!s_descriptors.contains(id))
		throw std::runtime_error("Invalid Struct TypeID");
	return s_descriptors[id];
}

Action<void, TypeID>& StructRegistry::on_struct_updated() {
	static Action<void, TypeID> action {};
	return action;
}

Action<void, TypeID>& StructRegistry::on_struct_removed() {
	static Action<void, TypeID> action {};
	return action;
}

}
