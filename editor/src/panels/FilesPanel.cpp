#include "panels/FilesPanel.h"

#include "origo/core/PathContext.h"

#include "systems/EditorIcons.h"

namespace OrigoEditor {

static bool tree_node_with_icon(
    const char* label,
    ImTextureID icon,
    ImGuiTreeNodeFlags flags) {

	ImGui::PushID(label);
	ImGui::PushStyleVar(
	    ImGuiStyleVar_FramePadding,
	    ImVec2(ImGui::GetStyle().FramePadding.x, 1.0f));

	bool open = ImGui::TreeNodeEx("##node", flags);
	ImGui::PopStyleVar();

	ImGui::SameLine();

	if (icon)
		ImGui::Image(icon, ImVec2(16, 16));
	else
		ImGui::Dummy(ImVec2(16, 16));

	ImGui::SameLine(0.0f, 6.0f);
	ImGui::TextUnformatted(label);

	ImGui::PopID();
	return open;
}

static void tree_leaf_with_icon(const char* label, ImTextureID icon) {
	ImGui::PushID(label);

	ImGui::TreeNodeEx(
	    "##leaf",
	    ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanFullWidth);

	ImGui::SameLine();

	if (icon)
		ImGui::Image(icon, ImVec2(16, 16));
	else
		ImGui::Dummy(ImVec2(16, 16));

	ImGui::SameLine(0.0f, 6.0f);
	ImGui::TextUnformatted(label);

	ImGui::PopID();
}

ImTextureID FilesPanel::get_icon_for_file(
    const std::filesystem::path& p) {

	const auto ext = p.extension();

	if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".bmp" || ext == ".tga")
		return EditorIcons::get_instance().get(IconType::Image);

	if (ext == ".cs" || ext == ".cpp" || ext == ".c" || ext == ".h" || ext == ".hpp" || ext == ".gd" || ext == ".glsl")
		return EditorIcons::get_instance().get(IconType::Script);

	return EditorIcons::get_instance().get(IconType::File);
}

bool FilesPanel::is_inside_project(
    const std::filesystem::path& p) {

	auto abs = std::filesystem::weakly_canonical(p);
	auto root = std::filesystem::weakly_canonical(m_state.ProjectRoot);

	auto abs_str = abs.generic_string();
	auto root_str = root.generic_string();

	return abs_str == root_str || (abs_str.starts_with(root_str) && abs_str[root_str.size()] == '/');
}

void FilesPanel::draw_file_system_recursive(
    const std::filesystem::path& path) {

	if (!is_inside_project(path))
		return;

	for (const auto& entry :
	    std::filesystem::directory_iterator(path)) {

		const auto p = entry.path();
		const auto name = p.filename().string();

		if (entry.is_directory()) {
			ImGuiTreeNodeFlags flags {
				ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_FramePadding
			};

			bool open = tree_node_with_icon(
			    name.c_str(),
			    EditorIcons::get_instance().get(IconType::Folder),
			    flags);

			if (ImGui::BeginDragDropSource(
			        ImGuiDragDropFlags_SourceAllowNullID)) {

				std::string s = p.string();
				ImGui::SetDragDropPayload(
				    "ASSET_PATH",
				    s.c_str(),
				    s.size() + 1);

				ImGui::TextUnformatted(name.c_str());
				ImGui::EndDragDropSource();
			}

			if (open) {
				draw_file_system_recursive(p);
				ImGui::TreePop();
			}
		} else if (entry.is_regular_file()) {
			if (p.extension() == ".asset" || p.extension() == ".meta")
				continue;

			tree_leaf_with_icon(
			    name.c_str(),
			    get_icon_for_file(p));

			if (ImGui::BeginDragDropSource(
			        ImGuiDragDropFlags_SourceAllowNullID)) {

				std::string s = p.string();
				ImGui::SetDragDropPayload(
				    "ASSET_PATH",
				    s.c_str(),
				    s.size() + 1);

				ImGui::TextUnformatted(name.c_str());
				ImGui::EndDragDropSource();
			}
		}
	}
}

void FilesPanel::on_im_gui_render() {
	if (m_state.ProjectRoot.empty())
		m_state.ProjectRoot = Origo::PathContextService::get_instance().project().Root;

	ImGui::BeginChild(
	    "FileSystemPanel",
	    ImVec2(0, 0),
	    true);

	ImGuiTreeNodeFlags root_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_FramePadding;

	bool open = tree_node_with_icon(
	    m_state.ProjectRoot.filename().string().c_str(),
	    EditorIcons::get_instance().get(IconType::Folder),
	    root_flags);

	if (open) {
		draw_file_system_recursive(m_state.ProjectRoot);
		ImGui::TreePop();
	}

	ImGui::EndChild();
}

}
