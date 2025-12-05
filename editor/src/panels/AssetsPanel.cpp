#include "panels/AssetsPanel.h"

#include "imgui.h"
#include <filesystem>
#include <string>

namespace OrigoEditor {

static std::filesystem::path s_ProjectRoot;
static std::filesystem::path s_SelectedFolder;
static bool s_Initialized = false;

static bool IsInsideProject(const std::filesystem::path& p) {
	auto abs = std::filesystem::weakly_canonical(p);
	auto root = std::filesystem::weakly_canonical(s_ProjectRoot);

	auto absStr = abs.generic_string();
	auto rootStr = root.generic_string();

	if (absStr.size() < rootStr.size())
		return false;

	return absStr.compare(0, rootStr.size(), rootStr) == 0;
}

static void DrawBreadcrumbs(const std::filesystem::path& path) {
	if (path.empty())
		return;

	auto rel = std::filesystem::relative(path, s_ProjectRoot);
	std::filesystem::path current = s_ProjectRoot;

	ImGui::SmallButton(s_ProjectRoot.filename().string().c_str());
	ImGui::SameLine();
	ImGui::TextUnformatted("/");

	bool first = true;

	for (auto& part : rel) {
		current /= part;

		ImGui::SameLine();
		if (ImGui::SmallButton(part.string().c_str()))
			s_SelectedFolder = current;

		ImGui::SameLine();
		ImGui::TextUnformatted("/");
	}

	ImGui::Separator();
}

static void DrawDirectoryTree(const std::filesystem::path& path) {
	if (path.empty())
		return;

	if (!IsInsideProject(path))
		return;

	ImGuiTreeNodeFlags baseFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_FramePadding;

	for (const auto& entry : std::filesystem::directory_iterator(path)) {
		if (!entry.is_directory())
			continue;

		const auto& p = entry.path();
		if (!IsInsideProject(p))
			continue;

		const auto& name = p.filename().string();

		ImGuiTreeNodeFlags flags = baseFlags;
		if (p == s_SelectedFolder)
			flags |= ImGuiTreeNodeFlags_Selected;

		bool open = ImGui::TreeNodeEx(name.c_str(), flags);

		if (ImGui::IsItemClicked())
			s_SelectedFolder = p;

		if (ImGui::BeginDragDropSource()) {
			std::string s = p.string();
			ImGui::SetDragDropPayload("ASSET_PATH", s.c_str(), s.size() + 1);
			ImGui::Text("%s", name.c_str());
			ImGui::EndDragDropSource();
		}

		if (open) {
			DrawDirectoryTree(p);
			ImGui::TreePop();
		}
	}
}

static void DrawFileGrid(const std::filesystem::path& folder) {
	if (!IsInsideProject(folder))
		return;

	const float cellSize = 96.0f;
	const float padding = 18.0f;

	float width = ImGui::GetContentRegionAvail().x;
	int columns = (int)(width / (cellSize + padding));
	if (columns < 1)
		columns = 1;

	ImGui::Columns(columns, nullptr, false);

	int index = 0;
	for (auto& entry : std::filesystem::directory_iterator(folder)) {
		const auto& p = entry.path();
		if (!IsInsideProject(p))
			continue;

		if (entry.is_directory()) {

			continue;
		}

		if (p.extension() == ".asset" || p.extension() == ".meta")
			continue;

		const auto filename = p.filename().string();

		ImGui::PushID(index++);
		ImGui::BeginGroup();

		ImGui::Button("ICON", ImVec2(cellSize, cellSize));

		if (ImGui::BeginDragDropSource()) {
			std::string s = p.string();
			ImGui::SetDragDropPayload("ASSET_PATH", s.c_str(), s.size() + 1);
			ImGui::Text("%s", filename.c_str());
			ImGui::EndDragDropSource();
		}

		ImGui::TextWrapped("%s", filename.c_str());
		ImGui::EndGroup();
		ImGui::PopID();

		ImGui::NextColumn();
	}

	ImGui::Columns(1);
}

void AssetsPanel::OnImGuiRender() {
	if (!s_Initialized) {
		s_ProjectRoot = std::filesystem::current_path();
		s_SelectedFolder = s_ProjectRoot;
		s_Initialized = true;
	}

	const float leftWidth = 260.0f;

	ImGui::BeginChild("LeftPane", ImVec2(leftWidth, 0), true);
	DrawDirectoryTree(s_ProjectRoot);
	ImGui::EndChild();

	ImGui::SameLine();

	ImGui::BeginChild("RightPane", ImVec2(0, 0), true);
	DrawBreadcrumbs(s_SelectedFolder);
	DrawFileGrid(s_SelectedFolder);
	ImGui::EndChild();

	if (ImGui::BeginPopupContextWindow("AssetsPanelContext", ImGuiPopupFlags_MouseButtonRight)) {
		if (ImGui::MenuItem("Create Folder")) { }
		if (ImGui::MenuItem("Import Asset")) { }
		if (ImGui::MenuItem("Delete")) { }
		ImGui::EndPopup();
	}
}

}
