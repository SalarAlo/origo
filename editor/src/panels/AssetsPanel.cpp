#include "panels/AssetsPanel.h"
#include "origo/assets/AssetManager.h"

#include "imgui.h"

namespace OrigoEditor {

void AssetsPanel::OnImGuiRender() {
	constexpr int COLS { 8 };
	const float height { 150.0f };

	if (ImGui::BeginTable("asset_table", COLS)) {
		for (const auto& [id, record] : Origo::AssetManager::GetRecords()) {
			std::string name = std::string { magic_enum::enum_name(record.AssetReference->GetAssetType()) };

			ImGui::TableNextColumn();
			if (ImGui::Button((name + "###" + id.ToString()).c_str(), { -FLT_MIN, height })) {
			}
		}
		ImGui::EndTable();
	}

	if (ImGui::BeginPopupContextWindow("AssetsPanelContext", ImGuiPopupFlags_MouseButtonRight)) {
		if (ImGui::MenuItem("Create Folder"))
			/* your logic */;

		if (ImGui::MenuItem("Import Asset"))
			/* your logic */;

		if (ImGui::MenuItem("Delete"))
			/* your logic */;

		ImGui::EndPopup();
	}
}
}
