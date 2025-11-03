#include "controllers/EditorUIPanelController.h"
#include "ui/UI.h"

namespace OrigoEditor {

void EditorUIPanelController::Render() {
	UI::BeginDockspace();
	UI::DrawMenuBar(m_Ctx.Panels);

	auto inspector = static_cast<InspectorPanel*>(m_Ctx.Panels.GetPanel(m_InspectorId));
	auto hierarchy = static_cast<HierarchyPanel*>(m_Ctx.Panels.GetPanel(m_HierarchyId));
	inspector->SetSelectedEntity(hierarchy->GetSelectedEntity());

	m_Ctx.Panels.RenderPanels();
	UI::EndDockspace();
}

}
