#pragma once
#include "panels/PanelManager.h"

namespace OrigoEditor::UI {
void ApplyEditorStyle();
void LoadEditorFont();

void BeginDockspace();
void DrawMenuBar(PanelManager& manager);
void EndDockspace();
}
