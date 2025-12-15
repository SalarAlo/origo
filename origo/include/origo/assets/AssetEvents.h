#pragma once

#include "origo/core/Action.h"
#include "origo/assets/AssetDescriptor.h"

namespace Origo {

class AssetEvents {
public:
	// When a new asset entry is created (metadata exists)
	inline static Action<void, const Metadata&> OnAssetCreated {};

	// When an asset's serialized data changes (.asset updated)
	inline static Action<void, const Metadata&> OnAssetModified {};

	// When an asset is removed (metadata + .asset deleted)
	inline static Action<void, const Metadata&> OnAssetDeleted {};

public:
	AssetEvents() = default;
	~AssetEvents() = default;

	// No copying — events must not duplicate callback lists
	AssetEvents(const AssetEvents&) = delete;
	AssetEvents& operator=(const AssetEvents&) = delete;

	AssetEvents(AssetEvents&&) = delete;
	AssetEvents& operator=(AssetEvents&&) = delete;
};

}
