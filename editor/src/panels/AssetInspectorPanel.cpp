#include "panels/AssetInspectorPanel.h"

#include "origo/assets/AssetDatabase.h"

#include "systems/AssetInspectorDrawer.h"

namespace OrigoEditor {

void AssetInspectorPanel::OnImGuiRender() {
	auto selectedAssetIDOptional = m_Context.GetSelectedAsset();
	auto activeScene = m_Context.ActiveScene;

	if (!selectedAssetIDOptional.has_value()) {
		ImGui::TextDisabled("No Asset selected.");
		ImGui::SetWindowFontScale(1.0f);
		return;
	}

	auto& selectedAssetID { *selectedAssetIDOptional };
	auto md { Origo::AssetDatabase::GetInstance().GetMetadata(selectedAssetID) };
	const std::string& name = md.Name;

	ImGui::SetWindowFontScale(1.1f);
	ImGui::SeparatorText(name.c_str());

	ImGui::SetWindowFontScale(0.9f);
	AssetInspectorDrawer::DrawAsset(md);
	ImGui::SetWindowFontScale(1.0f);
}

}
