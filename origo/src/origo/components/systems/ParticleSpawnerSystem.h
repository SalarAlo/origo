#pragma once

#include "origo/renderer/RenderContext.h"

#include "origo/scene/IRenderSystem.h"
#include "origo/scene/Scene.h"

namespace Origo {

class ParticleSpawnerSystem final : public Origo::IRenderSystem {
public:
	void Render(Origo::Scene* scene, RenderContext& rCtx) override;
};
}
