#pragma once

#include "origo/core/Layer.h"

#include "origo/renderer/RenderContext.h"

#include "state/EditorContext.h"

namespace OrigoEditor {

class UpdateLayer : public Origo::Layer {
public:
	UpdateLayer(EditorContext& ctx, Origo::RenderContext& rCtx)
	    : m_context(ctx)
	    , m_render_context(rCtx) { };

	void on_update(double dt) override;

private:
	EditorContext& m_context;
	Origo::RenderContext& m_render_context;
};

}
