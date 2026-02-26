#pragma once

#include "origo/renderer/RenderContext.h"
#include "state/EditorContext.h"
#include "origo/core/Layer.h"
#include "viewport/EditorViewportController.h"

namespace OrigoEditor {

class RenderLayer : public Origo::Layer {
public:
	RenderLayer(EditorContext& ctx, Origo::RenderContext& rCtx)
	    : m_context(ctx)
	    , m_render_context(rCtx) { };

	void on_attach() override;
	void on_update(double dt) override;

private:
	EditorContext& m_context;
	Origo::RenderContext& m_render_context;
};

}
