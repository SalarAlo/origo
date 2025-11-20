#include "panels/AssetsPanel.h"
#include "origo/assets/AssetManager.h"

#include "imgui.h"

namespace OrigoEditor {

void AssetsPanel::OnImGuiRender() {
	constexpr int COLS { 5 };
	const float height { 40.0f };

	if (ImGui::BeginTable("asset_table", COLS)) {
		for (const auto& [id, record] : Origo::AssetManager::GetRecords()) {
			std::string name { magic_enum::enum_name(record.AssetReference->GetAssetType()).data() };
			ImGui::TableNextColumn();
			if (ImGui::Button((name + "###" + id.ToString()).data(), ImVec2 { -FLT_MIN, height })) {
			}
		}

		ImGui::EndTable();
	}
}
}
