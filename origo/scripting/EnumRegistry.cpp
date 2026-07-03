#include <stdexcept>
#include <unordered_set>

#include "origo/scripting/EnumRegistry.h"

namespace Origo {

void EnumRegistry::replace_asset_enums(const UUID& asset_id, const std::vector<EnumMetadata>& metadata) {
	std::unordered_set<TypeID> incoming;
	for (const auto& item : metadata)
		incoming.insert(item.ID);

	std::vector<TypeID> removed;
	for (const auto& [id, desc] : s_descriptors) {
		if (desc.AssetID == asset_id && !incoming.contains(id))
			removed.push_back(id);
	}

	for (TypeID id : removed) {
		on_enum_removed().invoke(id);
		s_descriptors.erase(id);
	}

	for (const auto& item : metadata) {
		EnumDescriptor desc {};
		desc.ID = item.ID;
		desc.AssetID = item.AssetID;
		desc.Name = item.QualifiedName;
		desc.Version = item.Version;
		desc.Values = item.Values;
		desc.Annotations = item.Annotations;
		s_descriptors.insert_or_assign(desc.ID, std::move(desc));
		on_enum_updated().invoke(item.ID);
	}
}

bool EnumRegistry::exists(TypeID id) {
	return s_descriptors.contains(id);
}

const EnumDescriptor& EnumRegistry::get(TypeID id) {
	if (!s_descriptors.contains(id))
		throw std::runtime_error("Invalid Enum TypeID");
	return s_descriptors[id];
}

Action<void, TypeID>& EnumRegistry::on_enum_updated() {
	static Action<void, TypeID> action {};
	return action;
}

Action<void, TypeID>& EnumRegistry::on_enum_removed() {
	static Action<void, TypeID> action {};
	return action;
}

}
