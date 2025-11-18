#pragma once

#include "origo/renderer/RenderContext.h"
#include "origo/scene/Scene.h"

namespace Origo {
class RenderableRegistry {
public:
	using RenderFn = std::function<void(const Scene&, RenderContext&)>;

public:
	static RenderableRegistry& GetInstance() {
		static RenderableRegistry s_Instance {};
		return s_Instance;
	}

	void Register(RenderFn renderFn) {
		m_Renderers.emplace_back(std::move(renderFn));
	}

	const std::vector<RenderFn>& GetAllRenderers() { return m_Renderers; };

private:
	std::vector<RenderFn> m_Renderers {};
};
}

#define REGISTER_RENDERABLE(FN)                                         \
	struct AutoRenderableRegister_##FN {                            \
		AutoRenderableRegister_##FN() {                         \
			RenderableRegistry::GetInstance().Register(FN); \
		}                                                       \
	};                                                              \
	static AutoRenderableRegister_##FN s_AutoRenderableRegister_##FN;
