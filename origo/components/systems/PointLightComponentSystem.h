#pragma once

#include "origo/renderer/RenderContext.h"
#include "origo/scene/IRenderSystem.h"
#include "origo/scene/Scene.h"

namespace Origo {

class PointLightComponentSystem final : public Origo::IRenderSystem {
public:
	void render(Origo::Scene* scene, RenderContext& rCtx) override;
};
}
