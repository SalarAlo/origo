#include "TextEditorPanel.h"

#include <cmath>

#include "imgui.h"

#include "origo/assets/Asset.h"
#include "origo/assets/AssetManager.h"
#include "origo/assets/Script.h"

#include "origo/core/PathContext.h"

#include "systems/text_editor/TextEditableAsset.h"
#include "systems/text_editor/TextEditablePathVisitor.h"
#include "systems/text_editor/TextEditableSaveTextVisitor.h"

#include "ui/UI.h"

#include "zep.h"

namespace OrigoEditor {

namespace {

ImFont* get_fallback_font() {
	return UI::code_font ? UI::code_font : ImGui::GetFont();
}

int resolve_font_height() {
	const ImFont* font = get_fallback_font();
	return std::max(14, (int)std::round(font ? font->LegacySize : 16.0f));
}

bool is_reloadable_write_command(std::string_view command) {
	return command == ":w" || command == ":write";
}

}

struct ZepEditorHost final : public Zep::IZepComponent {
public:
	explicit ZepEditorHost(const std::filesystem::path& root, std::function<bool(std::string_view)> command_handler)
	    : m_editor(std::make_unique<Zep::ZepEditor_ImGui>(root, Zep::NVec2f(1.0f, 1.0f), Zep::ZepEditorFlags::DisableThreads)) {
		m_command_handler = std::move(command_handler);
		configure_fonts();
		m_editor->RegisterCallback(this);
		m_editor->GetConfig().style = Zep::EditorStyle::Normal;
		m_editor->GetConfig().showIndicatorRegion = false;
		m_editor->GetConfig().showLineNumbers = true;
		m_editor->GetConfig().showScrollBar = 1;
	}

	~ZepEditorHost() {
		if (m_editor)
			m_editor->UnRegisterCallback(this);
	}

	Zep::ZepEditor& GetEditor() const override { return *m_editor; }

	void Notify(std::shared_ptr<Zep::ZepMessage> message) override {
		if (message->messageId == Zep::Msg::GetClipBoard) {
			const char* clipboard = ImGui::GetClipboardText();
			message->str = clipboard ? clipboard : "";
			message->handled = true;
			return;
		}

		if (message->messageId == Zep::Msg::SetClipBoard) {
			ImGui::SetClipboardText(message->str.c_str());
			message->handled = true;
			return;
		}

		if (message->messageId == Zep::Msg::HandleCommand && m_command_handler) {
			message->handled = m_command_handler(message->str);
		}
	}

	void refresh_fonts_if_needed() {
		if (m_font_source == get_fallback_font())
			return;

		configure_fonts();
	}

	Zep::ZepBuffer* open_or_focus(const std::filesystem::path& path) {
		auto* buffer = m_editor->GetFileBuffer(path, 0, true);
		if (!buffer)
			return nullptr;

		m_editor->SetBufferSyntax(*buffer);

		if (auto* window = m_editor->EnsureWindow(*buffer)) {
			m_editor->SetCurrentWindow(window);
		}

		return buffer;
	}

	void render(const char* id) {
		refresh_fonts_if_needed();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::BeginChild(id, ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar);

		auto min = ImGui::GetCursorScreenPos();
		auto avail = ImGui::GetContentRegionAvail();
		auto max = ImVec2(min.x + std::max(1.0f, avail.x), min.y + std::max(1.0f, avail.y));

		m_editor->SetDisplayRegion(Zep::NVec2f(min.x, min.y), Zep::NVec2f(max.x, max.y));
		m_editor->Display();
		m_editor->HandleInput();

		ImGui::EndChild();
		ImGui::PopStyleVar();
	}

	Zep::ZepBuffer* active_buffer() const {
		return m_editor->GetActiveBuffer();
	}

private:
	void configure_fonts() {
		m_font_source = get_fallback_font();

		auto& display = static_cast<Zep::ZepDisplay_ImGui&>(m_editor->GetDisplay());
		const int font_height = resolve_font_height();
		auto zep_font = std::make_shared<Zep::ZepFont_ImGui>(display, m_font_source, font_height);

		display.SetFont(Zep::ZepTextType::UI, zep_font);
		display.SetFont(Zep::ZepTextType::Text, zep_font);
		display.SetFont(Zep::ZepTextType::Heading1, std::make_shared<Zep::ZepFont_ImGui>(display, m_font_source, (int)std::round(font_height * 1.75f)));
		display.SetFont(Zep::ZepTextType::Heading2, std::make_shared<Zep::ZepFont_ImGui>(display, m_font_source, (int)std::round(font_height * 1.5f)));
		display.SetFont(Zep::ZepTextType::Heading3, std::make_shared<Zep::ZepFont_ImGui>(display, m_font_source, (int)std::round(font_height * 1.25f)));
	}

private:
	std::unique_ptr<Zep::ZepEditor_ImGui> m_editor;
	std::function<bool(std::string_view)> m_command_handler;
	ImFont* m_font_source {};
};

TextEditorPanel::TextEditorPanel(EditorContext& ctx)
    : m_context(ctx)
    , m_zep(std::make_unique<ZepEditorHost>(
	          Origo::PathContextService::get_instance().project().Root,
	          [this](std::string_view command) {
		          return handle_editor_command(std::string(command));
	          })) { }

TextEditorPanel::~TextEditorPanel() = default;

void TextEditorPanel::on_im_gui_render() {
	static auto& am { Origo::AssetManager::get_instance() };

	auto uuid = m_context.get_selected_asset();
	if (!uuid) {
		m_current_path.clear();
		display_uneditable_asset("No asset selected");
		return;
	}

	auto handle = am.get_handle_by_uuid(*uuid);
	if (!handle) {
		m_current_path.clear();
		display_uneditable_asset("Invalid asset");
		return;
	}

	auto asset = am.get(*handle);

	auto editable = to_editable_asset(asset);
	if (!editable) {
		m_current_path.clear();
		std::string_view asset_name = magic_enum::enum_name(asset->get_asset_type());
		display_uneditable_asset(asset_name.data());
		return;
	}

	auto path = std::visit(TextEditablePathVisitor {}, *editable);
	if (!path) {
		m_current_path.clear();
		return;
	}

	render_editor(*editable, *path);

	if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows))
		m_context.claim_keyboard_input(KeyboardInputOwner::TextEditor);
}

void TextEditorPanel::save_current_asset(const TextEditableAsset& editable) {
	auto* buffer = m_zep ? m_zep->active_buffer() : nullptr;
	if (!buffer)
		return;

	auto text = buffer->GetBufferText(buffer->Begin(), buffer->End());
	std::visit(TextEditableSaveTextVisitor { text }, editable);
	buffer->ClearFileFlags(Zep::FileFlags::Dirty);
}

bool TextEditorPanel::handle_editor_command(const std::string& command) {
	if (!is_reloadable_write_command(command))
		return false;

	auto selected_asset = m_context.get_selected_asset();
	if (!selected_asset)
		return false;

	auto& asset_manager = Origo::AssetManager::get_instance();
	auto handle = asset_manager.get_handle_by_uuid(*selected_asset);
	if (!handle)
		return false;

	auto* asset = asset_manager.get(*handle);
	auto editable = to_editable_asset(asset);
	if (!editable)
		return false;

	save_current_asset(*editable);
	return true;
}

void TextEditorPanel::render_editor(const TextEditableAsset& editable, const std::filesystem::path& path) {
	if (!m_zep)
		return;

	if (m_current_path != path) {
		m_current_path = path;
		m_zep->open_or_focus(path);
	}

	const bool save_requested = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)
	    && ImGui::GetIO().KeyCtrl
	    && ImGui::IsKeyPressed(ImGuiKey_S, false);

	m_zep->render("ScriptEditor");

	if (save_requested)
		save_current_asset(editable);
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
