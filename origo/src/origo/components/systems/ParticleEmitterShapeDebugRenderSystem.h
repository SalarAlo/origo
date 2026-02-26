#pragma once

#include "origo/scene/IRenderSystem.h"

namespace Origo {
class ParticleEmitterShapeDebugRenderSystem final : public Origo::IRenderSystem {
public:
	void render(Origo::Scene* scene, RenderContext& rCtx) override;
};

}
