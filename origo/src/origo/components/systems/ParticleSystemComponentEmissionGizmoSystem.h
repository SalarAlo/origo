#pragma once

#include "origo/scene/IRenderSystem.h"

namespace Origo {
class ParticleSystemComponentEmissionGizmoSystem final : public Origo::IRenderSystem {
public:
	void Render(Origo::Scene* scene, RenderContext& rCtx) override;
};

}
