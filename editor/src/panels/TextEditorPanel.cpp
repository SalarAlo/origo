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
    : m_Context(ctx) {
	m_Editor.SetLanguageDefinition(TextEditor::LanguageDefinition::Lua());
}

void TextEditorPanel::OnImGuiRender() {
	static auto& am { Origo::AssetManager::GetInstance() };

	auto uuid = m_Context.GetSelectedAsset();
	if (!uuid) {
		m_CopiedFileContents = false;
		DisplayUneditableAsset("No asset selected");
		return;
	}

	auto handle = am.GetHandleByUUID(*uuid);
	if (!handle) {
		m_CopiedFileContents = false;
		DisplayUneditableAsset("Invalid asset");
		return;
	}

	auto asset = am.Get(*handle);

	auto editable = ToEditableAsset(asset);
	if (!editable) {
		m_CopiedFileContents = false;
		std::string_view asset_name = magic_enum::enum_name(asset->GetAssetType());
		DisplayUneditableAsset(asset_name.data());
		return;
	}

	auto path = std::visit(TextEditablePathVisitor {}, *editable);
	if (!path)
		return;

	if (!m_CopiedFileContents || m_CurrentPath != *path) {
		m_CurrentPath = *path;
		m_Editor.SetLanguageDefinition(TextEditor::LanguageDefinition::Lua());

		auto text = std::visit(TextEditableLoadTextVisitor {}, *editable);
		m_Editor.SetLanguageDefinition(std::visit(TextEditableSyntaxHighlit {}, *editable));
		m_Editor.SetText(text);

		m_CopiedFileContents = true;
	}

	if (UI::CodeFont) {
		ImGui::PushFont(UI::CodeFont);
		ImGui::SetWindowFontScale(1.15f);
	}

	m_Editor.Render("ScriptEditor");
	bool saveRequested = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_S);

	if (saveRequested)
		SaveCurrentAsset(*editable);

	if (UI::CodeFont) {
		ImGui::PopFont();
		ImGui::SetWindowFontScale(1.0f);
	}
}

void TextEditorPanel::SaveCurrentAsset(const TextEditableAsset& editable) {
	auto text { m_Editor.GetText() };
	std::visit(TextEditableSaveTextVisitor { text }, editable);
}

void TextEditorPanel::DisplayUneditableAsset(const std::string& assetTypeName) {
	ImGui::BeginChild("UneditableAssetPanel", ImVec2(0, 0), true);

	ImVec2 region = ImGui::GetContentRegionAvail();

	const char* icon = "\xee\x93\xaf";
	const char* title = "No Editable Source";
	const char* subtitle = "This asset cannot be modified directly";

	ImVec2 iconSize = ImGui::CalcTextSize(icon);
	ImVec2 titleSize = ImGui::CalcTextSize(title);
	ImVec2 subSize = ImGui::CalcTextSize(subtitle);
	ImVec2 typeSize = ImGui::CalcTextSize(assetTypeName.c_str());

	float totalHeight = iconSize.y + 12.0f + titleSize.y + 6.0f + subSize.y + 18.0f + typeSize.y;

	float startY = (region.y - totalHeight) * 0.5f;
	ImGui::SetCursorPosY(startY);

	auto centerText = [&](const char* txt) {
		float width = ImGui::CalcTextSize(txt).x;
		ImGui::SetCursorPosX((region.x - width) * 0.5f);
		ImGui::TextUnformatted(txt);
	};

	// Icon
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1));
	centerText(icon);
	ImGui::PopStyleColor();

	ImGui::Dummy(ImVec2(0, 12));

	// Title
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.85f, 0.85f, 0.85f, 1));
	centerText(title);
	ImGui::PopStyleColor();

	ImGui::Dummy(ImVec2(0, 6));

	// Subtitle
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, 1));
	centerText(subtitle);
	ImGui::PopStyleColor();

	ImGui::Dummy(ImVec2(0, 18));

	// Asset type label
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.4f, 0.4f, 1));
	centerText("Asset Type");
	ImGui::PopStyleColor();

	// Asset type value
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.55f, 0.35f, 1));
	centerText(assetTypeName.c_str());
	ImGui::PopStyleColor();

	ImGui::EndChild();
}

std::optional<TextEditableAsset> TextEditorPanel::ToEditableAsset(Origo::Asset* asset) {
	switch (asset->GetAssetType()) {
	case Origo::AssetType::Script:
		return static_cast<Origo::Script*>(asset);

	case Origo::AssetType::Shader:
		return static_cast<Origo::Shader*>(asset);

	default:
		return std::nullopt;
	}
}

}
