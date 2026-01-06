#include "origo/assets/Metadata.h"
#include "EditorAssetTree.h"
#include "systems/EditorIcons.h"

namespace OrigoEditor {
void EditorAssetTree::Build(const std::vector<Origo::AssetMetadata>& metadata) {
	m_Root = std::make_unique<FolderEntry>();
	m_Root->name = "Assets";
	m_Root->path = "";

	m_FolderMap.clear();
	m_FolderMap[""] = m_Root.get();

	for (const auto& md : metadata) {
		if (md.SourcePath.empty())
			continue;

		std::filesystem::path rel = md.SourcePath.lexically_normal();
		std::filesystem::path folderPath = rel.parent_path();

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

		auto asset = std::make_unique<AssetEntry>();
		asset->id = md.ID;
		asset->type = md.Type;
		asset->name = md.Name;
		asset->path = rel;
		asset->icon = EditorIcons::Get(md.Type);

		folder->assets.push_back(asset.get());
		m_Assets.push_back(std::move(asset));
	}
}
}
