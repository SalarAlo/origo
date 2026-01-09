#pragma once

#include "origo/assets/Asset.h"
#include "origo/core/UUID.h"

namespace Origo {

enum class AssetOrigin {
	Imported, // Has a source file (png, fbx, blablabla...)
	Runtime, // Created by the editor or at runtime (materials, prefabs, ...)
	Synthetic // Engine built asset (default shaders, fallback textures)
};

struct AssetMetadata {
	std::string Name {};
	OptionalUUID ID { std::nullopt };
	AssetType Type {};
	AssetOrigin Origin {};

	std::filesystem::path SourcePath {};

	std::filesystem::file_time_type SourceTimestamp {};
	std::filesystem::file_time_type ImportedTimestamp {};

	std::vector<UUID> Dependencies {};
};

};
