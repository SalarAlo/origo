#pragma once

#include "origo/renderer/RenderContext.h"
#include "state/EditorContext.h"
#include "origo/core/Layer.h"
#include "viewport/EditorViewportController.h"

namespace OrigoEditor {

class RenderLayer : public Origo::Layer {
public:
	RenderLayer(EditorContext& ctx, Origo::RenderContext& rCtx)
	    : m_Context(ctx)
	    , m_RenderContext(rCtx)
	    , m_ViewportController(ctx) { };

	void OnUpdate(double dt) override;

private:
	EditorContext& m_Context;
	Origo::RenderContext& m_RenderContext;
	EditorViewportController m_ViewportController;
};

}
