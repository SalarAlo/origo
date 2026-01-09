#include "EditorAssetTree.h"
#include "EditorAssetVirtualPath.h"
#include "panels/AssetThumbnailGenerator.h"

namespace OrigoEditor {

void EditorAssetTree::Build(const std::vector<Origo::AssetMetadata>& metadata) {
	m_Root = std::make_unique<FolderEntry>();
	m_Root->name = "Assets";
	m_Root->path = "";

	m_Folders.clear();
	m_Assets.clear();
	m_FolderMap.clear();

	m_FolderMap[""] = m_Root.get();

	for (const auto& md : metadata) {
		std::filesystem::path virtualPath = ComputeVirtualAssetPath(md);
		std::filesystem::path folderPath = virtualPath.parent_path();

		FolderEntry* folder = m_Root.get();
		std::filesystem::path current;

		for (const auto& part : folderPath) {
			current /= part;

			if (!m_FolderMap.contains(current)) {
				auto f = std::make_unique<FolderEntry>();
				f->name = part.string();
				f->path = current;

				FolderEntry* raw = f.get();
				m_Folders.push_back(std::move(f));
				m_FolderMap[current] = raw;
				folder->children.push_back(raw);
				folder = raw;
			} else {
				folder = m_FolderMap[current];
			}
		}

		auto asset = Origo::MakeScope<AssetEntry>();
		asset->id = *md.ID;
		asset->type = md.Type;
		asset->origin = md.Origin;
		asset->Name = md.Name;
		asset->virtualPath = virtualPath;
		asset->Icon = AssetThumbnailGenerator::GetThumbnailID(asset.get());

		folder->assets.push_back(asset.get());
		m_Assets.push_back(std::move(asset));
	}
}

}
