#include "panels/AssetsPanel.h"
#include "origo/assets/AssetManager.h"

#include "imgui.h"

namespace OrigoEditor {

void AssetsPanel::OnImGuiRender() {
	float width { ImGui::GetContentRegionAvail().x };
	float height { 40.0f };
	for (const auto& [id, record] : Origo::AssetManager::GetRecords()) {
		std::string name { magic_enum::enum_name(record.AssetReference->GetAssetType()).data() };
		if (ImGui::Button((name + "###" + id.ToString()).data(), ImVec2 { width, height })) {
		}
	}
}

}
