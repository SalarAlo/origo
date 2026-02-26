#pragma once

#include <ImGuizmo.h>
#include <TextEditor.h>

#include "panels/EditorPanel.h"

#include "state/EditorContext.h"

#include "systems/text_editor/TextEditableAsset.h"

namespace OrigoEditor {

class TextEditorPanel : public EditorPanel {
public:
	TextEditorPanel(EditorContext& ctx);

	void on_im_gui_render();
	const char* get_name() const { return "Text Editor"; }

private:
	void save_current_asset(const TextEditableAsset& editable);
	void display_uneditable_asset(const std::string& assetTypeName);
	std::optional<TextEditableAsset> to_editable_asset(Origo::Asset* asset);

private:
	EditorContext& m_context;
	TextEditor m_editor;

	bool m_copied_file_contents;
	std::filesystem::path m_current_path;
};

}
