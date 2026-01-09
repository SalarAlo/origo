#include "panels/AssetPanel.h"
#include "origo/assets/AssetDatabase.h"
#include "systems/EditorIcons.h"
#include <imgui.h>
#include <algorithm>
#include <string>
#include <cctype>
#include <misc/cpp/imgui_stdlib.h>

namespace OrigoEditor {

static std::string ClipTextToWidth(const std::string& text, float maxWidth) {
	if (ImGui::CalcTextSize(text.c_str()).x <= maxWidth)
		return text;

	std::string out = text;
	while (!out.empty()) {
		out.pop_back();
		std::string test = out + "...";
		if (ImGui::CalcTextSize(test.c_str()).x <= maxWidth)
			return test;
	}
	return "...";
}

static bool ContainsCaseInsensitive(const std::string& haystack, const std::string& needle) {
	if (needle.empty())
		return true;

	auto lower = [](unsigned char c) { return (char)std::tolower(c); };

	std::string h;
	h.reserve(haystack.size());
	for (char c : haystack)
		h.push_back(lower((unsigned char)c));

	std::string n;
	n.reserve(needle.size());
	for (char c : needle)
		n.push_back(lower((unsigned char)c));

	return h.find(n) != std::string::npos;
}

AssetPanel::AssetPanel(EditorContext& ctx)
    : m_Context(ctx) {
	const auto& metadataMap = Origo::AssetDatabase::GetAllMetadata();

	std::vector<Origo::AssetMetadata> snapshot;
	snapshot.reserve(metadataMap.size());
	for (const auto& [id, meta] : metadataMap)
		snapshot.push_back(meta);

	m_Tree.Build(snapshot);
	m_CurrentFolder = m_Tree.GetRoot();
	if (m_CurrentFolder)
		m_Path = { m_CurrentFolder };
}

void AssetPanel::OnImGuiRender() {
	ImGui::Begin("Assets", nullptr, ImGuiWindowFlags_NoScrollbar);

	DrawTopBar();

	ImGui::Separator();

	ImGui::BeginChild("AssetBrowser", ImVec2(0, 0), false);
	DrawFolderContents(m_CurrentFolder);
	ImGui::EndChild();

	ImGui::End();
}

void AssetPanel::DrawTopBar() {
	const float height = ImGui::GetFrameHeight() + ImGui::GetStyle().WindowPadding.y * 0.5f;
	ImGui::BeginChild("AssetTopBar", ImVec2(0, height), false, ImGuiWindowFlags_NoScrollbar);

	ImGuiStyle& style = ImGui::GetStyle();

	ImGui::SameLine();

	float searchWidth = 220.0f;
	float avail = ImGui::GetContentRegionAvail().x;
	float breadcrumbWidth = std::max(0.0f, avail - searchWidth - style.ItemSpacing.x);

	DrawBreadcrumbBar(breadcrumbWidth);

	ImGui::SameLine();

	ImGui::SetNextItemWidth(searchWidth);
	ImGui::InputTextWithHint("##AssetSearch", "Search...", &m_Search);

	ImGui::EndChild();
}

void AssetPanel::DrawBreadcrumbBar(float availableWidth) {
	if (!m_CurrentFolder)
		return;

	const float h = ImGui::GetFrameHeight();

	ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_HorizontalScrollbar;

	ImGui::BeginChild("Breadcrumb", ImVec2(availableWidth, h), false, flags);

	const float childH = h;

	const float itemH = ImGui::GetFrameHeight();
	const float availY = ImGui::GetContentRegionAvail().y;

	float yOffset = 0.0f;
	if (availY > itemH)
		yOffset = (availY - itemH) * 0.5f;

	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + yOffset);

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(6.0f, 0.0f));

	const ImU32 sepColor = ImGui::GetColorU32(ImGuiCol_TextDisabled);

	float totalWidth = 0.0f;
	for (size_t i = 0; i < m_Path.size(); ++i) {
		const char* label = m_Path[i]->name.empty() ? "Assets" : m_Path[i]->name.c_str();
		totalWidth += ImGui::CalcTextSize(label).x + ImGui::GetStyle().FramePadding.x * 2.0f;
		if (i + 1 < m_Path.size())
			totalWidth += ImGui::CalcTextSize(">").x + ImGui::GetStyle().ItemSpacing.x * 2.0f;
	}

	if (totalWidth > availableWidth)
		ImGui::SetScrollX(ImGui::GetScrollMaxX());

	for (size_t i = 0; i < m_Path.size(); ++i) {
		ImGui::PushID((int)i);

		const bool isLast = (i + 1 == m_Path.size());
		const char* rawLabel = m_Path[i]->name.empty() ? "Assets" : m_Path[i]->name.c_str();

		const float maxSegmentWidth = std::min(220.0f, availableWidth * 0.45f);
		std::string label = ClipTextToWidth(rawLabel, maxSegmentWidth);

		if (!isLast) {
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1, 1, 1, 0.06f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1, 1, 1, 0.10f));
		} else {
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 1, 1, 0.06f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1, 1, 1, 0.08f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1, 1, 1, 0.10f));
		}

		if (ImGui::Button(label.c_str())) {
			m_Path.resize(i + 1);
			m_CurrentFolder = m_Path.back();
		}

		ImGui::PopStyleColor(3);

		if (ImGui::IsItemHovered() && label != rawLabel) {
			ImGui::BeginTooltip();
			ImGui::TextUnformatted(rawLabel);
			ImGui::EndTooltip();
		}

		ImGui::PopID();

		if (i + 1 < m_Path.size()) {
			ImGui::SameLine();
			ImGui::PushStyleColor(ImGuiCol_Text, ImGui::ColorConvertU32ToFloat4(sepColor));
			ImGui::TextUnformatted(">");
			ImGui::PopStyleColor();
			ImGui::SameLine();
		}
	}

	ImGui::PopStyleVar(1);
	ImGui::EndChild();
}

void AssetPanel::DrawFolderContents(FolderEntry* folder) {
	if (!folder)
		return;

	constexpr float TILE_SIZE = 96.0f;
	constexpr float TILE_PADDING = 10.0f;
	constexpr float TEXT_HEIGHT = 22.0f;

	ImDrawList* drawList = ImGui::GetWindowDrawList();

	const float cellSize = TILE_SIZE + TILE_PADDING;
	const float panelWidth = ImGui::GetContentRegionAvail().x;
	const int columnCount = std::max(1, (int)(panelWidth / cellSize));

	ImGui::Columns(columnCount, nullptr, false);

	for (FolderEntry* child : folder->children) {
		if (!ContainsCaseInsensitive(child->name, m_Search))
			continue;
		DrawFolderTile(child, drawList);
		ImGui::NextColumn();
	}

	for (AssetEntry* asset : folder->assets) {
		if (!ContainsCaseInsensitive(asset->name, m_Search))
			continue;
		DrawAssetTile(asset, drawList);
		ImGui::NextColumn();
	}

	ImGui::Columns(1);
}

void AssetPanel::DrawFolderTile(FolderEntry* folder, ImDrawList* drawList) {
	constexpr float TILE_SIZE = 96.0f;
	constexpr float TEXT_HEIGHT = 22.0f;

	ImGui::PushID(folder);

	ImVec2 cursor = ImGui::GetCursorScreenPos();
	ImVec2 tileSize = { TILE_SIZE, TILE_SIZE + TEXT_HEIGHT };

	ImGui::InvisibleButton("folder_tile", tileSize);

	const bool hovered = ImGui::IsItemHovered();
	const bool clicked = ImGui::IsItemClicked(ImGuiMouseButton_Left);
	const bool doubleClicked = hovered && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left);

	ImU32 bgColor = hovered ? IM_COL32(80, 80, 80, 140) : IM_COL32(50, 50, 50, 90);
	drawList->AddRectFilled(cursor, cursor + tileSize, bgColor, 8.0f);

	ImVec2 thumbMin = cursor + ImVec2(10, 10);
	ImVec2 thumbMax = thumbMin + ImVec2(TILE_SIZE - 20, TILE_SIZE - 20);

	ImTextureID icon = EditorIcons::Get(IconType::Folder);
	if (icon)
		drawList->AddImage(icon, thumbMin, thumbMax);
	else
		drawList->AddRectFilled(thumbMin, thumbMax, IM_COL32(90, 90, 90, 255), 6.0f);

	const float textPadding = 6.0f;
	const float maxWidth = TILE_SIZE - textPadding * 2.0f;

	std::string displayName = ClipTextToWidth(folder->name, maxWidth);
	ImVec2 textSize = ImGui::CalcTextSize(displayName.c_str());
	const float fontSize = ImGui::GetFontSize();

	ImVec2 textPos = {
		cursor.x + (TILE_SIZE - textSize.x) * 0.5f,
		cursor.y + TILE_SIZE + (TEXT_HEIGHT - fontSize) * 0.5f
	};

	drawList->AddText(textPos, IM_COL32(230, 230, 230, 255), displayName.c_str());

	if (hovered && displayName != folder->name) {
		ImGui::BeginTooltip();
		ImGui::TextUnformatted(folder->name.c_str());
		ImGui::EndTooltip();
	}

	if (clicked || doubleClicked) {
		m_CurrentFolder = folder;
		m_Path.push_back(folder);
	}

	ImGui::PopID();
}

void AssetPanel::DrawAssetTile(AssetEntry* asset, ImDrawList* drawList) {
	constexpr float TILE_SIZE = 96.0f;
	constexpr float TEXT_HEIGHT = 22.0f;

	ImGui::PushID(&asset->id);

	ImVec2 cursor = ImGui::GetCursorScreenPos();
	ImVec2 tileSize = { TILE_SIZE, TILE_SIZE + TEXT_HEIGHT };

	ImGui::InvisibleButton("asset_tile", tileSize);

	if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		m_Context.SetSelectedAsset(*asset->id);

	const bool hovered = ImGui::IsItemHovered();

	const auto selectedAssetID = m_Context.GetSelectedAsset();
	const bool isSelected = (asset->id == selectedAssetID);

	ImU32 bgColor = isSelected ? IM_COL32(60, 120, 200, 180)
	    : hovered              ? IM_COL32(80, 80, 80, 140)
	                           : IM_COL32(50, 50, 50, 90);

	drawList->AddRectFilled(cursor, cursor + tileSize, bgColor, 8.0f);

	ImVec2 thumbMin = cursor + ImVec2(10, 10);
	ImVec2 thumbMax = thumbMin + ImVec2(TILE_SIZE - 20, TILE_SIZE - 20);

	if (asset->icon)
		drawList->AddImage(asset->icon, thumbMin, thumbMax);
	else
		drawList->AddRectFilled(thumbMin, thumbMax, IM_COL32(90, 90, 90, 255), 6.0f);

	const float textPadding = 6.0f;
	const float maxWidth = TILE_SIZE - textPadding * 2.0f;

	std::string displayName = ClipTextToWidth(asset->name, maxWidth);
	ImVec2 textSize = ImGui::CalcTextSize(displayName.c_str());
	const float fontSize = ImGui::GetFontSize();

	ImVec2 textPos = {
		cursor.x + (TILE_SIZE - textSize.x) * 0.5f,
		cursor.y + TILE_SIZE + (TEXT_HEIGHT - fontSize) * 0.5f
	};

	drawList->AddText(textPos, IM_COL32(230, 230, 230, 255), displayName.c_str());

	if (hovered && displayName != asset->name) {
		ImGui::BeginTooltip();
		ImGui::TextUnformatted(asset->name.c_str());
		ImGui::EndTooltip();
	}

	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
		const std::string uuidStr = (*asset->id).ToString();

		ImGui::TextUnformatted(asset->name.c_str());
		ImGui::SetDragDropPayload(
		    "ORIGO_ASSET_UUID",
		    uuidStr.c_str(),
		    uuidStr.size() + 1);

		ImGui::EndDragDropSource();
	}

	ImGui::PopID();
}

}
