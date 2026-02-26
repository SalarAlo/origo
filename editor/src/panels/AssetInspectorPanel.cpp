#include "panels/AssetInspectorPanel.h"

#include "origo/assets/AssetDatabase.h"

#include "systems/AssetInspectorDrawer.h"

namespace OrigoEditor {

void AssetInspectorPanel::on_im_gui_render() {
	auto selected_asset_id_optional = m_context.get_selected_asset();
	auto active_scene = m_context.ActiveScene;

	if (!selected_asset_id_optional.has_value()) {
		ImGui::TextDisabled("No Asset selected.");
		ImGui::SetWindowFontScale(1.0f);
		return;
	}

	auto& selected_asset_id { *selected_asset_id_optional };
	auto md { Origo::AssetDatabase::get_instance().get_metadata(selected_asset_id) };
	const std::string& name = md.Name;

	ImGui::SetWindowFontScale(1.1f);
	ImGui::SeparatorText(name.c_str());

	ImGui::SetWindowFontScale(0.9f);
	AssetInspectorDrawer::draw_asset(md);
	ImGui::SetWindowFontScale(1.0f);
}

}
