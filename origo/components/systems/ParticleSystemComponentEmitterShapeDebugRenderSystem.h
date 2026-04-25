#pragma once

#include "origo/scene/IRenderSystem.h"

namespace Origo {
class ParticleSystemComponentEmitterShapeDebugRenderSystem final : public Origo::IRenderSystem {
public:
	void render(Origo::Scene* scene, RenderContext& rCtx) override;
};

}
