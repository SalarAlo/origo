#pragma once
#include "panels/PanelManager.h"

#include "ui/EditorPalette.h"

namespace OrigoEditor::UI {
void apply_editor_style(const EditorPalette& p);
void load_editor_font();

void begin_dockspace();
void draw_menu_bar(PanelManager& manager, EditorContext& ctx);
void end_dockspace();

extern ImFont* ui_font;
extern ImFont* code_font;
}
