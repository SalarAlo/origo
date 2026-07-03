#pragma once

#include <unordered_map>
#include <vector>

#include "origo/core/Action.h"
#include "origo/scripting/FieldDescriptor.h"

namespace Origo {

struct StructDescriptor {
	TypeID ID { UUID::bad() };
	UUID AssetID { UUID::bad() };
	std::string Name {};
	uint32_t Version {};
	std::vector<FieldDescriptor> Fields {};
	std::vector<AnnotationMetadata> Annotations {};
};

class StructRegistry {
public:
	static void replace_asset_structs(const UUID& asset_id, const std::vector<StructMetadata>& metadata);
	static bool exists(TypeID id);
	static const StructDescriptor& get(TypeID id);
	static const std::unordered_map<TypeID, StructDescriptor>& get_all() { return s_descriptors; }
	static Action<void, TypeID>& on_struct_updated();
	static Action<void, TypeID>& on_struct_removed();

private:
	inline static std::unordered_map<TypeID, StructDescriptor> s_descriptors {};
};

}
