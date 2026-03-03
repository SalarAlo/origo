#include <imgui.h>

#include "panels/AssetInteraction.h"

#include "panels/AssetBrowserState.h"

namespace OrigoEditor {

void AssetInteraction::handle_folder_tile(FolderEntry* folder, AssetBrowserState& state, const EditorAssetTree& tree) const {
	const bool hovered = ImGui::IsItemHovered();
	const bool clicked = ImGui::IsItemClicked(ImGuiMouseButton_Left);
	const bool double_clicked = hovered && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left);

	if (clicked || double_clicked)
		state.navigate_to(folder, tree);
}

void AssetInteraction::handle_asset_tile(const AssetEntry& asset, EditorContext& ctx) const {
	if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		ctx.set_selected_asset(asset.Id);

	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
		const std::string uuid = asset.Id.to_string();

		ImGui::TextUnformatted(asset.Name.c_str());
		ImGui::SetDragDropPayload("ORIGO_ASSET_UUID", uuid.c_str(), uuid.size() + 1);
		ImGui::EndDragDropSource();
	}
}

bool AssetInteraction::is_asset_selected(const AssetEntry& asset, const EditorContext& context) const {
	const auto selected_asset_id = context.get_selected_asset();
	return selected_asset_id && asset.Id == *selected_asset_id;
}

}
