#pragma once

#include "origo/scene/IRenderSystem.h"

namespace Origo {

class TerrainComponentRenderSystem : public IRenderSystem {
public:
	void render(Scene* scene, RenderContext& context) override;
};

}
