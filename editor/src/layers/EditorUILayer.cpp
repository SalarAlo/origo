#include "layer/EditorUILayer.h"
#include "panels/AssetsPanel.h"
#include "panels/ConsolePanel.h"
#include "panels/HierarchyPanel.h"
#include "panels/InspectorPanel.h"
#include "panels/SceneViewport.h"
#include "ui/UI.h"

namespace OrigoEditor {

EditorUILayer::EditorUILayer(EditorContext& ctx)
    : m_Context(ctx)
    , m_PanelManager(ctx) {
}

void EditorUILayer::OnDetach() {
	m_ImGuiController.OnDetach();
}

void EditorUILayer::OnAttach() {
	m_ImGuiController.OnAttach(m_Context.Window);

	UI::ApplyEditorStyle(m_Context.ColorPalette);
	UI::LoadEditorFont();

	m_PanelManager.AddPanel<HierarchyPanel>(m_Context);
	m_PanelManager.AddPanel<InspectorPanel>(m_Context);
	m_PanelManager.AddPanel<SceneViewport>(m_Context);
	m_PanelManager.AddPanel<AssetsPanel>();

	m_PanelManager.AddPanel<ConsolePanel>();
}

void EditorUILayer::OnUpdate(double dt) {
	m_ImGuiController.Begin();

	UI::BeginDockspace();
	UI::DrawMenuBar(m_PanelManager);

	m_PanelManager.RenderPanels();

	UI::EndDockspace();

	m_ImGuiController.End();
}
void EditorUILayer::OnEvent(Origo::Event& e) {
}

}
