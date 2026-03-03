#include <cctype>

#include "panels/AssetBrowserState.h"

namespace OrigoEditor {

void AssetBrowserState::restore(const EditorAssetTree& tree, const std::filesystem::path& previous_path) {
	m_current_folder = tree.find_folder(previous_path);
	if (!m_current_folder)
		m_current_folder = tree.get_root();

	m_path.clear();
	tree.build_path(m_current_folder, m_path);
}

void AssetBrowserState::navigate_to(FolderEntry* folder, const EditorAssetTree& tree) {
	if (!folder)
		return;

	m_current_folder = folder;
	m_path.clear();
	tree.build_path(m_current_folder, m_path);
}

void AssetBrowserState::navigate_to_breadcrumb(size_t index) {
	if (index >= m_path.size())
		return;

	m_path.resize(index + 1);
	m_current_folder = m_path.back();
}

bool AssetBrowserState::matches_search(const std::string& value) const {
	if (m_search.empty())
		return true;

	auto lower = [](unsigned char c) { return (char)std::tolower(c); };

	std::string haystack;
	haystack.reserve(value.size());
	for (char c : value)
		haystack.push_back(lower((unsigned char)c));

	std::string needle;
	needle.reserve(m_search.size());
	for (char c : m_search)
		needle.push_back(lower((unsigned char)c));

	return haystack.find(needle) != std::string::npos;
}

}
