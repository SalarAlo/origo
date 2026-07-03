#pragma once

#include <unordered_map>
#include <vector>

#include "origo/core/Action.h"
#include "origo/scripting/ReflectionMetadata.h"

namespace Origo {

struct EnumDescriptor {
	TypeID ID { UUID::bad() };
	UUID AssetID { UUID::bad() };
	std::string Name {};
	uint32_t Version {};
	std::vector<EnumValueMetadata> Values {};
	std::vector<AnnotationMetadata> Annotations {};
};

class EnumRegistry {
public:
	static void replace_asset_enums(const UUID& asset_id, const std::vector<EnumMetadata>& metadata);
	static bool exists(TypeID id);
	static const EnumDescriptor& get(TypeID id);
	static const std::unordered_map<TypeID, EnumDescriptor>& get_all() { return s_descriptors; }
	static Action<void, TypeID>& on_enum_updated();
	static Action<void, TypeID>& on_enum_removed();

private:
	inline static std::unordered_map<TypeID, EnumDescriptor> s_descriptors {};
};

}
