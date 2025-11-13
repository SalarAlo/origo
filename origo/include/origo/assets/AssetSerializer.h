#pragma once
#include "Asset.h"
#include "origo/serialization/ISerializer.h"

namespace Origo {

class AssetSerializer {
public:
	virtual ~AssetSerializer() = default;
	virtual bool ShouldSerialize(const Asset* asset) const { return true; };
	virtual void Serialize(const Asset* asset, ISerializer& backend) const = 0;
	virtual Asset* Deserialize(ISerializer& backend) const = 0;
};

namespace AssetSerializationSystem {
	void Register(AssetType type, AssetSerializer* serializer);
	AssetSerializer* Get(AssetType type);

	void RegisterAllAssetSerializers();
	void Cleanup();

	void SaveAll(ISerializer& serializer);
	void LoadAll(ISerializer& backend);
}

// idk if i wanna use this because of... SIOF
#define REGISTER_SERIALIZER(NAME)                                                   \
	namespace {                                                                 \
		const bool s_##NAME##_serializer_registered = [] {                  \
			Origo::AssetSerializationSystem::Register(                  \
			    Origo::AssetType::NAME, new Origo::NAME##Serializer()); \
			return true;                                                \
		}();                                                                \
	}

}
