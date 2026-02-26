#include "layers/EditorUILayer.h"

#include "panels/AssetInspectorPanel.h"
#include "panels/AssetPanel.h"
#include "panels/ConsolePanel.h"
#include "panels/DebugStatsPanel.h"
#include "panels/EntityInspectorPanel.h"
#include "panels/HierarchyPanel.h"
#include "panels/RuntimeStatePanel.h"
#include "panels/SceneViewport.h"
#include "panels/TextEditorPanel.h"

#include "ui/UI.h"

namespace OrigoEditor {

EditorUILayer::EditorUILayer(EditorContext& ctx, Origo::ImGuiLayer& controller)
    : m_context(ctx)
    , m_im_gui_controller(controller)
    , m_panel_manager(ctx) {
}

void EditorUILayer::on_detach() {
	m_im_gui_controller.on_detach();
}

void EditorUILayer::on_attach() {
	m_im_gui_controller.on_attach(m_context.Window);

	UI::apply_editor_style(m_context.ColorPalette);
	UI::load_editor_font();

	m_panel_manager.add_panel<SceneViewport>(m_context);
	m_panel_manager.add_panel<HierarchyPanel>(m_context);
	m_panel_manager.add_panel<EntityInspectorPanel>(m_context);
	m_panel_manager.add_panel<ConsolePanel>();
	m_panel_manager.add_panel<AssetInspectorPanel>(m_context);
	m_panel_manager.add_panel<AssetPanel>(m_context);
	m_panel_manager.add_panel<TextEditorPanel>(m_context);
	m_panel_manager.add_panel<RuntimeStatePanel>(m_context);
	m_panel_manager.add_panel<DebugStatsPanel>(m_context);
	// m_PanelManager.AddPanel<FilesPanel>();
}

void EditorUILayer::on_update(double dt) {
	m_im_gui_controller.begin();

	UI::begin_dockspace();
	UI::draw_menu_bar(m_panel_manager, m_context);

	m_panel_manager.render_panels();

	UI::end_dockspace();

	m_im_gui_controller.end();
}
void EditorUILayer::on_event(Origo::Event& e) {
}

}
