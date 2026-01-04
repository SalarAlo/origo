#pragma once

#include "origo/assets/AssetManagerFast.h"
#include "origo/renderer/RenderContext.h"
#include "state/EditorContext.h"
#include "origo/core/Layer.h"
#include "viewport/EditorViewportController.h"

namespace OrigoEditor {

class RenderLayer : public Origo::Layer {
public:
	RenderLayer(EditorContext& ctx, Origo::RenderContext& rCtx)
	    : m_Context(ctx)
	    , m_RenderContext(rCtx) { };

	void OnAttach() override;
	void OnUpdate(double dt) override;

private:
	EditorContext& m_Context;
	Origo::RenderContext& m_RenderContext;

	Origo::AssetHandle m_SkyboxCubeMesh;
	Origo::AssetHandle m_SkyboxMaterial;
};

}
