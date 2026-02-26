#include "EditorAssetTree.h"
#include "EditorAssetVirtualPath.h"
#include "panels/AssetThumbnailGenerator.h"

namespace OrigoEditor {

void EditorAssetTree::build(const std::vector<Origo::AssetMetadata>& metadata) {
	m_root = std::make_unique<FolderEntry>();
	m_root->Name = "Assets";
	m_root->Path = "";

	m_folders.clear();
	m_assets.clear();
	m_folder_map.clear();

	m_folder_map[""] = m_root.get();

	for (const auto& md : metadata) {
		std::filesystem::path virtual_path = compute_virtual_asset_path(md);
		std::filesystem::path folder_path = virtual_path.parent_path();

		FolderEntry* folder = m_root.get();
		std::filesystem::path current;

		for (const auto& part : folder_path) {
			current /= part;

			if (!m_folder_map.contains(current)) {
				auto f = std::make_unique<FolderEntry>();
				f->Name = part.string();
				f->Path = current;

				FolderEntry* raw = f.get();
				m_folders.push_back(std::move(f));
				m_folder_map[current] = raw;
				folder->Children.push_back(raw);
				folder = raw;
			} else {
				folder = m_folder_map[current];
			}
		}

		auto asset = Origo::MakeScope<AssetEntry>();
		asset->Id = *md.ID;
		asset->Type = md.Type;
		asset->Origin = md.Origin;
		asset->Name = md.Name;
		asset->VirtualPath = virtual_path;
		asset->Icon = AssetThumbnailGenerator::get_thumbnail_id(asset.get());

		folder->Assets.push_back(asset.get());
		m_assets.push_back(std::move(asset));
	}
}

}
