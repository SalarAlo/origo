#pragma once
#include "Asset.h"
#include "origo/serialization/ISerializer.h"

namespace Origo {

class AssetSerializer {
public:
	virtual void Serialize(const Ref<Asset>& asset, ISerializer& backend) const = 0;
	virtual Ref<Asset> Deserialize(ISerializer& backend) const = 0;
};

namespace AssetSerializationSystem {
	void Register(AssetType type, Ref<AssetSerializer> serializer);
	Ref<AssetSerializer> Get(AssetType type);

	void RegisterAllAssetSerializers();

	void SaveAll(ISerializer& serializer);
	void LoadAll(ISerializer& backend);
}

// idk if i wanna use this because of... SIOF
#define REGISTER_SERIALIZER(NAME)                                                               \
	namespace {                                                                             \
		const bool s_##NAME##_serializer_registered = [] {                              \
			Origo::AssetSerializationSystem::Register(                              \
			    Origo::AssetType::NAME, Origo::MakeRef<Origo::NAME##Serializer>()); \
			return true;                                                            \
		}();                                                                            \
	}

}
