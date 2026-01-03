#include "panels/FilesPanel.h"

#include "systems/EditorIcons.h"

namespace OrigoEditor {

static bool TreeNodeWithIcon(
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

static void TreeLeafWithIcon(const char* label, ImTextureID icon) {
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

ImTextureID FilesPanel::GetIconForFile(
    const std::filesystem::path& p) {

	const auto ext = p.extension();

	if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".bmp" || ext == ".tga")
		return EditorIcons::Get(IconType::Image);

	if (ext == ".cs" || ext == ".cpp" || ext == ".c" || ext == ".h" || ext == ".hpp" || ext == ".gd" || ext == ".glsl")
		return EditorIcons::Get(IconType::Script);

	return EditorIcons::Get(IconType::File);
}

bool FilesPanel::IsInsideProject(
    const std::filesystem::path& p) {

	auto abs = std::filesystem::weakly_canonical(p);
	auto root = std::filesystem::weakly_canonical(m_State.ProjectRoot);

	auto absStr = abs.generic_string();
	auto rootStr = root.generic_string();

	return absStr == rootStr || (absStr.starts_with(rootStr) && absStr[rootStr.size()] == '/');
}

void FilesPanel::DrawFileSystemRecursive(
    const std::filesystem::path& path) {

	if (!IsInsideProject(path))
		return;

	for (const auto& entry :
	    std::filesystem::directory_iterator(path)) {

		const auto p = entry.path();
		const auto name = p.filename().string();

		if (entry.is_directory()) {
			ImGuiTreeNodeFlags flags {
				ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_FramePadding
			};

			bool open = TreeNodeWithIcon(
			    name.c_str(),
			    EditorIcons::Get(IconType::Folder),
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
				DrawFileSystemRecursive(p);
				ImGui::TreePop();
			}
		} else if (entry.is_regular_file()) {
			if (p.extension() == ".asset" || p.extension() == ".meta")
				continue;

			TreeLeafWithIcon(
			    name.c_str(),
			    GetIconForFile(p));

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

void FilesPanel::OnImGuiRender() {
	if (m_State.ProjectRoot == "./")
		m_State.ProjectRoot = std::filesystem::current_path();

	ImGui::BeginChild(
	    "FileSystemPanel",
	    ImVec2(0, 0),
	    true);

	ImGuiTreeNodeFlags rootFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_FramePadding;

	bool open = TreeNodeWithIcon(
	    m_State.ProjectRoot.filename().string().c_str(),
	    EditorIcons::Get(IconType::Folder),
	    rootFlags);

	if (open) {
		DrawFileSystemRecursive(m_State.ProjectRoot);
		ImGui::TreePop();
	}

	ImGui::EndChild();
}

}
