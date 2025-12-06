#pragma once

#include "origo/assets/AssetMetadata.h"

namespace Origo {
class AssetDatabase {
public:
	static void WriteMetadata(const AssetMetadata& meta);
	static void WriteAssetdata(const UUID& uuid);
	static void RegisterMetadata(const AssetMetadata& meta);
	static AssetMetadata LoadMetadata(const std::filesystem::path& path);
	static Asset* LoadAsset(const UUID& id);

public:
	const inline static std::filesystem::path ROOT { "./assets" };
	const inline static std::filesystem::path ROOT_RESOURCES { "./resources" };

private:
	static std::filesystem::path GetMetadataPath(const AssetMetadata& id);
	static std::filesystem::path GetAssetPath(const AssetMetadata& id);

private:
	inline static std::unordered_map<UUID, AssetMetadata> s_Metadata {};
};
}
