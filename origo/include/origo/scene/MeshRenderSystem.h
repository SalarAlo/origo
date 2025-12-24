#pragma once

#include "origo/scene/IRenderSystem.h"
#include "origo/scene/Scene.h"
#include "origo/renderer/RenderContext.h"

namespace Origo {

class MeshRenderSystem final : public IRenderSystem {
public:
	void Render(Scene* scene, RenderContext& context) override;
};

}
