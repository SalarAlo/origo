#pragma once

#include <ankerl/unordered_dense.h>

#include "origo/assets/Metadata.h"

#include "origo/core/UUID.h"

namespace OrigoEditor {

struct AssetEntry {
	AssetEntry() = default;
	Origo::UUID Id { Origo::UUID::generate() };
	OptionalUUID ParentId { std::nullopt };
	Origo::AssetType Type;
	Origo::AssetOrigin Origin;
	std::string Name;
	std::filesystem::path VirtualPath;
	ImTextureID Icon;
};

struct FolderEntry {
	std::string Name;
	std::filesystem::path Path;
	std::vector<FolderEntry*> Children;
	std::vector<AssetEntry*> Assets;
};

class EditorAssetTree {
public:
	void build(const std::vector<Origo::AssetMetadata>& metadata);
	FolderEntry* get_root() const { return m_root.get(); }
	FolderEntry* find_folder(const std::filesystem::path& path) const;
	void build_path(FolderEntry* folder, std::vector<FolderEntry*>& outPath) const;

private:
	std::unique_ptr<FolderEntry> m_root;
	std::vector<std::unique_ptr<FolderEntry>> m_folders;
	std::vector<std::unique_ptr<AssetEntry>> m_assets;
	ankerl::unordered_dense::map<std::filesystem::path, FolderEntry*> m_folder_map;
};

}
