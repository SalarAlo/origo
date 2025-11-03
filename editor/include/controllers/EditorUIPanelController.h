#include "controllers/EditorContext.h"
#include "panels/AssetsPanel.h"
#include "panels/ConsolePanel.h"
#include "panels/HierarchyPanel.h"
#include "panels/InspectorPanel.h"
#include "panels/SceneViewport.h"

namespace OrigoEditor {
class EditorUIPanelController {
public:
	explicit EditorUIPanelController(EditorContext& ctx)
	    : m_Ctx(ctx) {
		m_HierarchyId = ctx.Panels.AddPanel<HierarchyPanel>(ctx.Scene);
		m_InspectorId = ctx.Panels.AddPanel<InspectorPanel>(ctx.Scene);
		ctx.Panels.AddPanel<SceneViewport>(ctx.Buffer, ctx.Camera);
		ctx.Panels.AddPanel<AssetsPanel>();
		ctx.Panels.AddPanel<ConsolePanel>();
	}

	void Render();

private:
	EditorContext& m_Ctx;
	int m_InspectorId {};
	int m_HierarchyId {};
};

}
