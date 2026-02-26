#pragma once

#include "origo/renderer/RenderContext.h"

#include "origo/scene/IRenderSystem.h"
#include "origo/scene/Scene.h"

namespace Origo {

class ModelRenderSystem final : public IRenderSystem {
public:
	void render(Scene* scene, RenderContext& context) override;
};

}
