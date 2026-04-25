#pragma once

#include "origo/assets/Asset.h"

#include "origo/core/UUID.h"

namespace Origo {

enum class AssetOrigin {
	Imported, // External data requiring transformation into runtime form (payload stored in .import)
	Authored, // Engine native data resolved directly from source (.import holds metadata only)
	Runtime, // Created by the editor or at runtime (materials for materials, meshes for models, ...)
	Synthetic // Engine built asset (default shaders, fallback textures)
};

struct AssetMetadata {
	std::string Name {};
	OptionalUUID ID { std::nullopt };
	OptionalUUID ParentID { std::nullopt };
	AssetType Type {};
	AssetOrigin Origin {};

	std::optional<std::filesystem::path> SourcePath {};

	std::filesystem::file_time_type SourceTimestamp {};
	std::filesystem::file_time_type ImportedTimestamp {};

	std::vector<UUID> Dependencies {};
};

};
