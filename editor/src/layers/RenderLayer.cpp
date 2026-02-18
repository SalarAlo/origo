#include "layers/RenderLayer.h"

#include "origo/assets/SkyboxDefaults.h"

#include "origo/scene/GamePhase.h"
#include "origo/scene/SystemScheduler.h"

using namespace Origo;

namespace OrigoEditor {

void RenderLayer::OnAttach() {
	m_RenderContext.SetSkyboxMaterial(SkyboxDefaults::GetMaterial());
}

void RenderLayer::OnUpdate(double dt) {
	const bool editingView = (m_Context.ViewMode == EditorViewMode::Editor);

	m_RenderContext.SetView(m_Context.ViewportController.GetActiveRenderView());

	m_RenderContext.BeginFrame();

	auto activeScenePtr = m_Context.ActiveScene;

	SystemScheduler::Get().RunPhase(GamePhase::RenderAlways, activeScenePtr, m_RenderContext);
	SystemScheduler::Get().RunPhase(GamePhase::UpdateAlways, activeScenePtr, dt);

	if (editingView) {
		SystemScheduler::Get().RunPhase(GamePhase::RenderEditor, activeScenePtr, m_RenderContext);
	}

	m_RenderContext.Flush();

	m_RenderContext.EndFrame();
}

}
