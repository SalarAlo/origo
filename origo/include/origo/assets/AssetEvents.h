#pragma once

#include "origo/core/Action.h"
#include "origo/assets/AssetDescriptor.h"

namespace Origo {

class AssetEvents {
public:
	inline static Action<void, const Metadata&> OnAssetCreated {};
	inline static Action<void, const Metadata&> OnAssetModified {};
	inline static Action<void, const Metadata&> OnAssetDeleted {};

public:
	AssetEvents() = default;
	~AssetEvents() = default;

	AssetEvents(const AssetEvents&) = delete;
	AssetEvents& operator=(const AssetEvents&) = delete;

	AssetEvents(AssetEvents&&) = delete;
	AssetEvents& operator=(AssetEvents&&) = delete;
};

}
