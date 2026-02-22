
#pragma once

#include "origo/assets/Metadata.h"

#include "origo/utils/Singleton.h"

namespace Origo {

class Asset;

class AssetDatabase : public Singleton<AssetDatabase> {
public:
	const std::unordered_map<UUID, AssetMetadata>& GetAllMetadata() { return s_Metadata; }
	void WriteImportFile(const UUID& id);
	const AssetMetadata& GetMetadata(const UUID& id);
	void RegisterMetadata(const AssetMetadata& meta);
	AssetMetadata LoadImportHeader(const std::filesystem::path& path);
	Asset* LoadAsset(const UUID& id);
	void SaveAll();

public:
	const std::filesystem::path ROOT { "./assets" };

private:
	std::filesystem::path GetImportPath(const AssetMetadata& meta);

private:
	std::unordered_map<UUID, AssetMetadata> s_Metadata {};
};

}
