#include "origo/assets/AssetDatabase.h"
#include "origo/assets/Asset.h"
#include "origo/assets/AssetManager.h"
#include "origo/assets/AssetMetadata.h"

namespace Origo {

void AssetDatabase::Initialise() {
	AssetManager::SubscribeToAssetCreated([](RID rid, AssetType type) {
		auto asset { AssetManager::GetAssetName(rid) };
		AssetMetadata meta;

		meta.Id = UUID();
		meta.Type = type;
	});
}

}
