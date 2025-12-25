
#include "layer/RenderLayer.h"
#include "origo/scene/GamePhase.h"
#include "origo/scene/SystemScheduler.h"
#include "state/EditorRuntimeState.h"

using namespace Origo;

namespace OrigoEditor {

void RenderLayer::OnUpdate(double dt) {
	m_RenderContext.SetView(m_ViewportController.GetActiveRenderView());

	m_RenderContext.BeginFrame();

	auto scenePtr = m_Context.ActiveScene;

	if (m_Context.RuntimeState == EditorRuntimeState::Running) {
		SystemScheduler::Get().RunPhase(GamePhase::Update, scenePtr, dt);
	}

	SystemScheduler::Get().RunPhase(GamePhase::RenderGeometry, scenePtr, m_RenderContext);

	if (m_Context.RuntimeState == EditorRuntimeState::Editing) {
		SystemScheduler::Get().RunPhase(GamePhase::RenderEditor, scenePtr, m_RenderContext);
	}

	m_RenderContext.Flush();

	m_RenderContext.EndFrame();
}

}
