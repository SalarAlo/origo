#include "origo/scene/SystemScheduler.h"

namespace Origo {

void SystemScheduler::run_phase(GamePhase phase, Scene* scene, float dt) {
	auto it = m_update_systems.find(phase);

	if (it == m_update_systems.end())
		return;

	for (auto& sys : it->second)
		sys->update(scene, dt);
}

void SystemScheduler::run_phase(GamePhase phase, Scene* scene, RenderContext& ctx) {
	auto it = m_render_systems.find(phase);

	if (it == m_render_systems.end())
		return;

	for (auto& sys : it->second)
		sys->render(scene, ctx);
}

void SystemScheduler::add_update_system(GamePhase phase, Scope<IUpdateSystem> system) {
	m_update_systems[phase].push_back(std::move(system));
}

void SystemScheduler::add_render_system(GamePhase phase, Scope<IRenderSystem> system) {
	m_render_systems[phase].push_back(std::move(system));
}

}
