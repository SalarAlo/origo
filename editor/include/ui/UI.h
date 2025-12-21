#pragma once
#include "panels/PanelManager.h"
#include "ui/EditorPalette.h"

namespace OrigoEditor::UI {
void ApplyEditorStyle(const EditorPalette& p);
void LoadEditorFont();

void BeginDockspace();
void DrawMenuBar(PanelManager& manager);
void EndDockspace();
}
