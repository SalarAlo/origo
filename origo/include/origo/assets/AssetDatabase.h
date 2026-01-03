
#pragma once

#include "origo/assets/Metadata.h"

namespace Origo {

class Asset;

class AssetDatabase {
public:
	static void WriteImportFile(const UUID& id);
	static const AssetMetadata& GetMetadata(const UUID& id);
	static void RegisterMetadata(const AssetMetadata& meta);
	static AssetMetadata LoadImportHeader(const std::filesystem::path& path);
	static Asset* LoadAsset(const UUID& id);
	static void SaveAll();

public:
	const inline static std::filesystem::path ROOT { "./assets" };

private:
	static std::filesystem::path GetImportPath(const AssetMetadata& meta);

private:
	inline static std::unordered_map<UUID, AssetMetadata> s_Metadata {};
};

}
