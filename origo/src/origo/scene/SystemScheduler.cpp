#include "origo/scene/SystemScheduler.h"

namespace Origo {

void SystemScheduler::RunPhase(GamePhase phase, Scene* scene, float dt) {
	auto it = m_UpdateSystems.find(phase);
	if (it == m_UpdateSystems.end())
		return;

	for (auto& sys : it->second)
		sys->Update(scene, dt);
}

void SystemScheduler::RunPhase(GamePhase phase, Scene* scene, RenderContext& ctx) {
	auto it = m_RenderSystems.find(phase);
	if (it == m_RenderSystems.end())
		return;

	for (auto& sys : it->second)
		sys->Render(scene, ctx);
}

void SystemScheduler::AddUpdateSystem(GamePhase phase, Scope<IUpdateSystem> system) {
	m_UpdateSystems[phase].push_back(std::move(system));
}

void SystemScheduler::AddRenderSystem(GamePhase phase, Scope<IRenderSystem> system) {
	m_RenderSystems[phase].push_back(std::move(system));
}
}
