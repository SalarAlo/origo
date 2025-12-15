#pragma once

#include "origo/assets/Asset.h"
#include "origo/core/UUID.h"

namespace Origo {

enum class AssetOrigin {
	Imported, // Has a source file (png, fbx, blablabla...)
	Generated, // Created by the editor or at runtime (materials, prefabs, ...)
	Internal // Engine built asset (default shaders, fallback textures)
};

struct Metadata {
	std::string Name {};
	UUID Id { UUID::Bad() };
	AssetType Type {};
	AssetOrigin Origin {};

	std::filesystem::path SourcePath {};

	std::string ImporterName {};
	std::filesystem::file_time_type SourceTimestamp {};

	std::vector<UUID> Dependencies {};
};

};
