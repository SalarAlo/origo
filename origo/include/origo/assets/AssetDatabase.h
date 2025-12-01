#pragma once

#include "origo/assets/AssetMetadata.h"

namespace Origo {
class AssetDatabase {
public:
	static void WriteMetadata(const AssetMetadata& meta);

private:
	static std::filesystem::path GetMetadataPath(const AssetMetadata& id);

private:
	inline static std::filesystem::path s_Root { "./assets" };
	inline static std::unordered_map<UUID, AssetMetadata> s_Metadata {};
};
}
