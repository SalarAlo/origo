
#pragma once

#include "origo/assets/AssetDescriptor.h"

namespace Origo {

class Asset;

class AssetDatabase {
public:
	static void WriteImportFile(const UUID& id);
	static void RegisterMetadata(const Metadata& meta);
	static Metadata LoadImportHeader(const std::filesystem::path& path);
	static Asset* LoadAsset(const UUID& id);
	static void SaveAll();

public:
	const inline static std::filesystem::path ROOT { "./assets" };

private:
	static std::filesystem::path GetImportPath(const Metadata& meta);

private:
	inline static std::unordered_map<UUID, Metadata> s_Metadata {};
};

}
