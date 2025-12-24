#pragma once

#include "origo/scene/IRenderSystem.h"
#include "origo/scene/Scene.h"
#include "origo/renderer/RenderContext.h"

namespace OrigoEditor {

class EditorOutlineRenderSystem final : public Origo::IRenderSystem {
public:
	void Render(Origo::Scene* scene, Origo::RenderContext& context) override;
};

}
