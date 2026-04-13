#pragma once

#include <ImGuizmo.h>

#include <memory>

#include "panels/EditorPanel.h"

#include "state/EditorContext.h"

#include "systems/text_editor/TextEditableAsset.h"

namespace OrigoEditor {

struct ZepEditorHost;

class TextEditorPanel : public EditorPanel {
public:
	TextEditorPanel(EditorContext& ctx);
	~TextEditorPanel() override;

	void on_im_gui_render() override;
	const char* get_name() const override { return "Text Editor"; }

private:
	bool handle_editor_command(const std::string& command);
	void save_current_asset(const TextEditableAsset& editable);
	void render_editor(const TextEditableAsset& editable, const std::filesystem::path& path);
	void display_uneditable_asset(const std::string& assetTypeName);
	std::optional<TextEditableAsset> to_editable_asset(Origo::Asset* asset);

private:
	EditorContext& m_context;
	std::unique_ptr<ZepEditorHost> m_zep;
	std::filesystem::path m_current_path {};
};

}
