
#pragma once

#include "origo/assets/AssetDescriptor.h"
#include <filesystem>
#include <unordered_map>

namespace Origo {

class Asset;

class AssetDatabase {
public:
	static void WriteImport(const UUID& id);
	static void RegisterMetadata(const AssetDescriptor& meta);
	static AssetDescriptor LoadImportHeader(const std::filesystem::path& path);
	static Asset* LoadAsset(const UUID& id);
	static void SaveAll();

public:
	const inline static std::filesystem::path ROOT { "./assets" };

private:
	static std::filesystem::path GetImportPath(const AssetDescriptor& meta);

private:
	inline static std::unordered_map<UUID, AssetDescriptor> s_Metadata {};
};

}
