#pragma once

#include "origo/scene/IRenderSystem.h"

namespace Origo {

class TerrainRenderSystem final : public IRenderSystem {
public:
	void render(Scene* scene, RenderContext& context) override;
};

}
