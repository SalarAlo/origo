#pragma once

#include "origo/scene/GamePhase.h"
#include "origo/scene/IRenderSystem.h"
#include "origo/scene/IUpdateSystem.h"

namespace Origo {

class SystemScheduler {
public:
	static SystemScheduler& Get() {
		static SystemScheduler instance;
		return instance;
	}

	void RunPhase(GamePhase phase, Scene& scene, float dt);
	void RunPhase(GamePhase phase, const Scene& scene, RenderContext& ctx);

	void AddUpdateSystem(GamePhase phase, Scope<IUpdateSystem> system);
	void AddRenderSystem(GamePhase phase, Scope<IRenderSystem> system);

private:
	std::unordered_map<GamePhase, std::vector<Scope<IUpdateSystem>>> m_UpdateSystems;
	std::unordered_map<GamePhase, std::vector<Scope<IRenderSystem>>> m_RenderSystems;
};

}

#define REGISTER_UPDATE_SYSTEM(PHASE, TYPE) \
	REGISTER_UPDATE_SYSTEM_IMPL(PHASE, TYPE, __COUNTER__)

#define REGISTER_UPDATE_SYSTEM_IMPL(PHASE, TYPE, N)                            \
	namespace {                                                            \
		struct ORIGO_CONCAT(AutoRegister_UpdateSystem_, N) {           \
			ORIGO_CONCAT(AutoRegister_UpdateSystem_, N)() {        \
				Origo::SystemScheduler::Get().AddUpdateSystem( \
				    (PHASE), std::make_unique<TYPE>());        \
			}                                                      \
		};                                                             \
		static ORIGO_CONCAT(AutoRegister_UpdateSystem_, N)             \
		    ORIGO_CONCAT(s_AutoRegister_UpdateSystem_, N);             \
	}
#define ORIGO_CONCAT_IMPL(x, y) x##y
#define ORIGO_CONCAT(x, y) ORIGO_CONCAT_IMPL(x, y)

#define REGISTER_RENDER_SYSTEM(PHASE, TYPE) \
	REGISTER_RENDER_SYSTEM_IMPL(PHASE, TYPE, __COUNTER__)

#define REGISTER_RENDER_SYSTEM_IMPL(PHASE, TYPE, N)                            \
	namespace {                                                            \
		struct ORIGO_CONCAT(AutoRegister_RenderSystem_, N) {           \
			ORIGO_CONCAT(AutoRegister_RenderSystem_, N)() {        \
				Origo::SystemScheduler::Get().AddRenderSystem( \
				    (PHASE), std::make_unique<TYPE>());        \
			}                                                      \
		};                                                             \
		static ORIGO_CONCAT(AutoRegister_RenderSystem_, N)             \
		    ORIGO_CONCAT(s_AutoRegister_RenderSystem_, N);             \
	}
