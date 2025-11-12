#include "layer/EditorUILayer.h"
#include "panels/AssetsPanel.h"
#include "panels/ConsolePanel.h"
#include "panels/HierarchyPanel.h"
#include "panels/InspectorPanel.h"
#include "panels/SceneViewport.h"
#include "ui/UI.h"

namespace OrigoEditor {

EditorUILayer::EditorUILayer(EditorContext& ctx)
    : m_Context(ctx) {
}

void EditorUILayer::OnDetach() {
	m_ImGuiController.OnDetach();
}

void EditorUILayer::OnAttach() {
	m_ImGuiController.OnAttach(m_Context.Window);

	UI::ApplyEditorStyle();
	UI::LoadEditorFont();

	m_PanelManager.AddPanel<HierarchyPanel>(m_Context);
	m_PanelManager.AddPanel<InspectorPanel>(m_Context);
	m_PanelManager.AddPanel<SceneViewport>(m_Context.Buffer, *m_Context.Scene.GetMainCamera());
	m_PanelManager.AddPanel<AssetsPanel>();

	m_PanelManager.AddPanel<ConsolePanel>();
}

void EditorUILayer::OnUpdate(double dt) {
	m_ImGuiController.Begin();

	UI::BeginDockspace();
	UI::DrawMenuBar(m_PanelManager);

	auto inspector = static_cast<InspectorPanel*>(m_PanelManager.GetPanel(m_InspectorId));
	auto hierarchy = static_cast<HierarchyPanel*>(m_PanelManager.GetPanel(m_HierarchyId));

	m_PanelManager.RenderPanels();

	UI::EndDockspace();

	m_ImGuiController.End();
}
void EditorUILayer::OnEvent(Origo::Event& e) {
}

}
