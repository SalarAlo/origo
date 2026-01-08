#pragma once

#include <filesystem>
#include <unordered_map>
#include <vector>

#include "origo/assets/Metadata.h"
#include "origo/core/UUID.h"
#include "origo/assets/Asset.h"

namespace OrigoEditor {

struct AssetEntry {
	Origo::UUID id;
	Origo::AssetType type;
	std::string name;
	std::filesystem::path path;
	ImTextureID icon;
};

struct FolderEntry {
	std::string name;
	std::filesystem::path path;

	std::vector<FolderEntry*> children;
	std::vector<AssetEntry*> assets;

	bool open = false;
};

class EditorAssetTree {
public:
	void Build(const std::vector<Origo::AssetMetadata>& metadata);
	FolderEntry* GetRoot() { return m_Root.get(); }

private:
	std::unique_ptr<FolderEntry> m_Root;
	std::vector<std::unique_ptr<FolderEntry>> m_Folders;
	std::vector<std::unique_ptr<AssetEntry>> m_Assets;

	std::unordered_map<std::filesystem::path, FolderEntry*> m_FolderMap;
};

}
