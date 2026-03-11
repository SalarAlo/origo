#include "TextEditorPanel.h"

#include "imgui.h"

#include "origo/assets/Asset.h"
#include "origo/assets/AssetManager.h"
#include "origo/assets/Script.h"

#include "systems/text_editor/TextEditableAsset.h"
#include "systems/text_editor/TextEditableLoadTextVisitor.h"
#include "systems/text_editor/TextEditablePathVisitor.h"
#include "systems/text_editor/TextEditableSaveTextVisitor.h"
#include "systems/text_editor/TextEditableSyntaxHighlit.h"

#include "ui/UI.h"

namespace OrigoEditor {

TextEditorPanel::TextEditorPanel(EditorContext& ctx)
    : m_context(ctx) {
	m_editor.SetLanguageDefinition(TextEditor::LanguageDefinition::Lua());
}

void TextEditorPanel::on_im_gui_render() {
	static auto& am { Origo::AssetManager::get_instance() };

	auto uuid = m_context.get_selected_asset();
	if (!uuid) {
		m_copied_file_contents = false;
		display_uneditable_asset("No asset selected");
		return;
	}

	auto handle = am.get_handle_by_uuid(*uuid);
	if (!handle) {
		m_copied_file_contents = false;
		display_uneditable_asset("Invalid asset");
		return;
	}

	auto asset = am.get(*handle);

	auto editable = to_editable_asset(asset);
	if (!editable) {
		m_copied_file_contents = false;
		std::string_view asset_name = magic_enum::enum_name(asset->get_asset_type());
		display_uneditable_asset(asset_name.data());
		return;
	}

	auto path = std::visit(TextEditablePathVisitor {}, *editable);
	if (!path)
		return;

	if (!m_copied_file_contents || m_current_path != *path) {
		m_current_path = *path;
		m_editor.SetLanguageDefinition(TextEditor::LanguageDefinition::Lua());

		auto text = std::visit(TextEditableLoadTextVisitor {}, *editable);
		m_editor.SetLanguageDefinition(std::visit(TextEditableSyntaxHighlit {}, *editable));
		m_editor.SetText(text);

		m_copied_file_contents = true;
	}

	if (UI::code_font) {
		ImGui::PushFont(UI::code_font);
		ImGui::SetWindowFontScale(1.15f);
	}

	m_editor.Render("ScriptEditor");
	if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && ImGui::GetIO().WantTextInput)
		m_context.mark_text_input_active();

	bool save_requested = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_S);

	if (save_requested)
		save_current_asset(*editable);

	if (UI::code_font) {
		ImGui::PopFont();
		ImGui::SetWindowFontScale(1.0f);
	}
}

void TextEditorPanel::save_current_asset(const TextEditableAsset& editable) {
	auto text { m_editor.GetText() };
	std::visit(TextEditableSaveTextVisitor { text }, editable);
}

void TextEditorPanel::display_uneditable_asset(const std::string& assetTypeName) {
	ImGui::BeginChild("UneditableAssetPanel", ImVec2(0, 0), true);

	ImVec2 region = ImGui::GetContentRegionAvail();

	const char* icon = "\xee\x93\xaf";
	const char* title = "No Editable Source";
	const char* subtitle = "This asset cannot be modified directly";

	ImVec2 icon_size = ImGui::CalcTextSize(icon);
	ImVec2 title_size = ImGui::CalcTextSize(title);
	ImVec2 sub_size = ImGui::CalcTextSize(subtitle);
	ImVec2 type_size = ImGui::CalcTextSize(assetTypeName.c_str());

	float total_height = icon_size.y + 12.0f + title_size.y + 6.0f + sub_size.y + 18.0f + type_size.y;

	float start_y = (region.y - total_height) * 0.5f;
	ImGui::SetCursorPosY(start_y);

	auto center_text = [&](const char* txt) {
		float width = ImGui::CalcTextSize(txt).x;
		ImGui::SetCursorPosX((region.x - width) * 0.5f);
		ImGui::TextUnformatted(txt);
	};

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1));
	center_text(icon);
	ImGui::PopStyleColor();

	ImGui::Dummy(ImVec2(0, 12));

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.85f, 0.85f, 0.85f, 1));
	center_text(title);
	ImGui::PopStyleColor();

	ImGui::Dummy(ImVec2(0, 6));

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, 1));
	center_text(subtitle);
	ImGui::PopStyleColor();

	ImGui::Dummy(ImVec2(0, 18));

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.4f, 0.4f, 1));
	center_text("Asset Type");
	ImGui::PopStyleColor();

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.55f, 0.35f, 1));
	center_text(assetTypeName.c_str());
	ImGui::PopStyleColor();

	ImGui::EndChild();
}

std::optional<TextEditableAsset> TextEditorPanel::to_editable_asset(Origo::Asset* asset) {
	switch (asset->get_asset_type()) {
	case Origo::AssetType::Script:
		return static_cast<Origo::Script*>(asset);

	case Origo::AssetType::Shader:
		return static_cast<Origo::Shader*>(asset);

	default:
		return std::nullopt;
	}
}

}
