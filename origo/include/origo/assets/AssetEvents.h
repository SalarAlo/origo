#pragma once

#include "origo/core/Action.h"
#include "origo/assets/Metadata.h"

namespace Origo {

class AssetEvents {
public:
	inline static Action<void, const AssetMetadata&> OnAssetCreated {};
	inline static Action<void, const AssetMetadata&> OnAssetModified {};
	inline static Action<void, const AssetMetadata&> OnAssetDeleted {};

public:
	AssetEvents() = default;
	~AssetEvents() = default;

	AssetEvents(const AssetEvents&) = delete;
	AssetEvents& operator=(const AssetEvents&) = delete;

	AssetEvents(AssetEvents&&) = delete;
	AssetEvents& operator=(AssetEvents&&) = delete;
};

}
