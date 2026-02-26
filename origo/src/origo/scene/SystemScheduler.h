#pragma once

#include "origo/scene/GamePhase.h"
#include "origo/scene/IRenderSystem.h"
#include "origo/scene/IUpdateSystem.h"

namespace Origo {

class SystemScheduler {
public:
	static SystemScheduler& get() {
		static SystemScheduler instance;
		return instance;
	}

	void run_phase(GamePhase phase, Scene* scene, float dt);
	void run_phase(GamePhase phase, Scene* scene, RenderContext& ctx);

	void add_update_system(GamePhase phase, Scope<IUpdateSystem> system);
	void add_render_system(GamePhase phase, Scope<IRenderSystem> system);

private:
	std::unordered_map<GamePhase, std::vector<Scope<IUpdateSystem>>> m_update_systems;
	std::unordered_map<GamePhase, std::vector<Scope<IRenderSystem>>> m_render_systems;
};

}

#define REGISTER_UPDATE_SYSTEM(PHASE, TYPE) \
	REGISTER_UPDATE_SYSTEM_IMPL(PHASE, TYPE, __COUNTER__)

#define REGISTER_UPDATE_SYSTEM_IMPL(PHASE, TYPE, N)                              \
	namespace {                                                              \
		struct ORIGO_CONCAT(AutoRegister_UpdateSystem_, N) {             \
			ORIGO_CONCAT(AutoRegister_UpdateSystem_, N)() {          \
				Origo::SystemScheduler::get().add_update_system( \
				    (PHASE), std::make_unique<TYPE>());          \
			}                                                        \
		};                                                               \
		static ORIGO_CONCAT(AutoRegister_UpdateSystem_, N)               \
		    ORIGO_CONCAT(s_AutoRegister_UpdateSystem_, N);               \
	}

#define ORIGO_CONCAT_IMPL(x, y) x##y
#define ORIGO_CONCAT(x, y) ORIGO_CONCAT_IMPL(x, y)

#define REGISTER_RENDER_SYSTEM(PHASE, TYPE) \
	REGISTER_RENDER_SYSTEM_IMPL(PHASE, TYPE, __COUNTER__)

#define REGISTER_RENDER_SYSTEM_IMPL(PHASE, TYPE, N)                              \
	namespace {                                                              \
		struct ORIGO_CONCAT(AutoRegister_RenderSystem_, N) {             \
			ORIGO_CONCAT(AutoRegister_RenderSystem_, N)() {          \
				Origo::SystemScheduler::get().add_render_system( \
				    (PHASE), std::make_unique<TYPE>());          \
			}                                                        \
		};                                                               \
		static ORIGO_CONCAT(AutoRegister_RenderSystem_, N)               \
		    ORIGO_CONCAT(s_AutoRegister_RenderSystem_, N);               \
	}
