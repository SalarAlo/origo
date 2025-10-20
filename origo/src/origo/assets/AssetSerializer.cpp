#include "origo/assets/AssetSerializer.h"
#include <unordered_map>

namespace Origo {

static std::unordered_map<AssetType, Ref<AssetSerializer>> s_AssetSerializer;

namespace AssetSerializationRegistry {
	void Register(AssetType type, Ref<AssetSerializer> serializer) {
		s_AssetSerializer[type] = serializer;
	}

	Ref<AssetSerializer> Get(AssetType type) {
		if (s_AssetSerializer.find(type) == s_AssetSerializer.end())
			return nullptr;

		return s_AssetSerializer[type];
	}
}

}
