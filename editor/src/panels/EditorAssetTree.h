#pragma once

#include <ankerl/unordered_dense.h>
#include "origo/assets/Metadata.h"
#include "origo/core/UUID.h"

namespace OrigoEditor {

struct AssetEntry {
	AssetEntry() = default;
	Origo::UUID id { Origo::UUID::Generate() };
	Origo::AssetType type;
	Origo::AssetOrigin origin;
	std::string Name;
	std::filesystem::path virtualPath;
	ImTextureID Icon;
};

struct FolderEntry {
	std::string name;
	std::filesystem::path path;
	std::vector<FolderEntry*> children;
	std::vector<AssetEntry*> assets;
};

class EditorAssetTree {
public:
	void Build(const std::vector<Origo::AssetMetadata>& metadata);
	FolderEntry* GetRoot() const { return m_Root.get(); }

private:
	std::unique_ptr<FolderEntry> m_Root;
	std::vector<std::unique_ptr<FolderEntry>> m_Folders;
	std::vector<std::unique_ptr<AssetEntry>> m_Assets;
	ankerl::unordered_dense::map<std::filesystem::path, FolderEntry*> m_FolderMap;
};

}
