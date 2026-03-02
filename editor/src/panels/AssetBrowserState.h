#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include "panels/EditorAssetTree.h"

namespace OrigoEditor {

class AssetBrowserState {
public:
	void restore(const EditorAssetTree& tree, const std::filesystem::path& previous_path);
	void navigate_to(FolderEntry* folder, const EditorAssetTree& tree);
	void navigate_to_breadcrumb(size_t index);

	FolderEntry* get_current_folder() const { return m_current_folder; }
	const std::vector<FolderEntry*>& get_path() const { return m_path; }

	std::string& search() { return m_search; }
	const std::string& search() const { return m_search; }

	bool matches_search(const std::string& value) const;

private:
	FolderEntry* m_current_folder { nullptr };
	std::vector<FolderEntry*> m_path {};
	std::string m_search {};
};

}
