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

	void OnImGuiRender();
	const char* GetName() const { return "Text Editor"; }

private:
	void SaveCurrentAsset(const TextEditableAsset& editable);
	void DisplayUneditableAsset(const std::string& assetTypeName);
	std::optional<TextEditableAsset> ToEditableAsset(Origo::Asset* asset);

private:
	EditorContext& m_Context;
	TextEditor m_Editor;

	bool m_CopiedFileContents;
	std::filesystem::path m_CurrentPath;
};

}
