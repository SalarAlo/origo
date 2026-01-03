#include "panels/AssetPanel.h"
#include "origo/assets/AssetDatabase.h"
#include "origo/assets/AssetManagerFast.h"
#include "systems/EditorIcons.h"
#include <imgui.h>

namespace OrigoEditor {

void AssetPanel::OnImGuiRender() {
	constexpr float TILE_SIZE = 96.0f;
	constexpr float TILE_PADDING = 8.0f;
	constexpr float TEXT_HEIGHT = 20.0f;

	ImDrawList* drawList = ImGui::GetWindowDrawList();

	const float cellSize = TILE_SIZE + TILE_PADDING;
	const float panelWidth = ImGui::GetContentRegionAvail().x;
	const int columnCount = std::max(1, (int)(panelWidth / cellSize));

	ImGui::Columns(columnCount, nullptr, false);

	for (const auto& [uuid, assetHandle] :
	    Origo::AssetManagerFast::GetInstance().GetUuidMap()) {

		auto md = Origo::AssetDatabase::GetMetadata(uuid);
		bool isSelected = (uuid == selectedAssetID);

		ImGui::PushID(uuid.ToString().c_str());

		ImVec2 cursor = ImGui::GetCursorScreenPos();
		ImVec2 tileSize = { TILE_SIZE, TILE_SIZE + TEXT_HEIGHT };

		ImGui::InvisibleButton("asset_tile", tileSize);

		bool hovered = ImGui::IsItemHovered();
		bool clicked = ImGui::IsItemClicked();

		if (clicked)
			selectedAssetID = uuid;

		ImU32 bgColor = isSelected ? IM_COL32(60, 120, 200, 180) : hovered ? IM_COL32(80, 80, 80, 120)
		                                                                   : IM_COL32(50, 50, 50, 80);

		drawList->AddRectFilled(
		    cursor,
		    cursor + tileSize,
		    bgColor,
		    6.0f);

		ImVec2 thumbMin = cursor + ImVec2(8, 8);
		ImVec2 thumbMax = thumbMin + ImVec2(TILE_SIZE - 16, TILE_SIZE - 16);

		ImTextureID icon = EditorIcons::Get(md.Type);
		if (icon) {
			drawList->AddImage(
			    icon,
			    thumbMin,
			    thumbMax);
		} else {
			drawList->AddRectFilled(
			    thumbMin,
			    thumbMax,
			    IM_COL32(90, 90, 90, 255),
			    4.0f);
		}

		const float textPadding = 6.0f;
		const float maxWidth = TILE_SIZE - textPadding * 2.0f;
		const float fontSize = ImGui::GetFontSize();

		std::string displayName = md.Name;
		if (ImGui::CalcTextSize(displayName.c_str()).x > maxWidth) {
			while (!displayName.empty()) {
				displayName.pop_back();
				if (ImGui::CalcTextSize((displayName + "...").c_str()).x <= maxWidth)
					break;
			}
			displayName += "...";
		}

		ImVec2 textSize = ImGui::CalcTextSize(displayName.c_str());

		ImVec2 textPos = {
			cursor.x + (TILE_SIZE - textSize.x) * 0.5f,
			cursor.y + TILE_SIZE + (TEXT_HEIGHT - fontSize) * 0.5f
		};

		drawList->AddText(
		    textPos,
		    IM_COL32(220, 220, 220, 255),
		    displayName.c_str());

		if (hovered && displayName != md.Name) {
			ImGui::BeginTooltip();
			ImGui::TextUnformatted(md.Name.c_str());
			ImGui::EndTooltip();
		}

		ImGui::NextColumn();
		ImGui::PopID();
	}

	ImGui::Columns(1);
}

}
