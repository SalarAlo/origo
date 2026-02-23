#include "layers/EditorUILayer.h"

#include "panels/AssetInspectorPanel.h"
#include "panels/AssetPanel.h"
#include "panels/ConsolePanel.h"
#include "panels/EntityInspectorPanel.h"
#include "panels/HierarchyPanel.h"
#include "panels/RuntimeStatePanel.h"
#include "panels/SceneViewport.h"
#include "panels/TextEditorPanel.h"

#include "ui/UI.h"

namespace OrigoEditor {

EditorUILayer::EditorUILayer(EditorContext& ctx, Origo::ImGuiLayer& controller)
    : m_Context(ctx)
    , m_ImGuiController(controller)
    , m_PanelManager(ctx) {
}

void EditorUILayer::OnDetach() {
	m_ImGuiController.OnDetach();
}

void EditorUILayer::OnAttach() {
	m_ImGuiController.OnAttach(m_Context.Window);

	UI::ApplyEditorStyle(m_Context.ColorPalette);
	UI::LoadEditorFont();

	m_PanelManager.AddPanel<SceneViewport>(m_Context);
	m_PanelManager.AddPanel<HierarchyPanel>(m_Context);
	m_PanelManager.AddPanel<EntityInspectorPanel>(m_Context);
	m_PanelManager.AddPanel<ConsolePanel>();
	m_PanelManager.AddPanel<AssetInspectorPanel>(m_Context);
	m_PanelManager.AddPanel<AssetPanel>(m_Context);
	m_PanelManager.AddPanel<TextEditorPanel>(m_Context);
	m_PanelManager.AddPanel<RuntimeStatePanel>(m_Context);
	// m_PanelManager.AddPanel<FilesPanel>();
}

void EditorUILayer::OnUpdate(double dt) {
	m_ImGuiController.Begin();

	UI::BeginDockspace();
	UI::DrawMenuBar(m_PanelManager, m_Context);

	m_PanelManager.RenderPanels();

	UI::EndDockspace();

	m_ImGuiController.End();
}
void EditorUILayer::OnEvent(Origo::Event& e) {
}

}
