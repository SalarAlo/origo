#pragma once

#include "origo/core/Layer.h"

#include "origo/renderer/RenderContext.h"

#include "state/EditorContext.h"

namespace OrigoEditor {

class UpdateLayer : public Origo::Layer {
public:
	UpdateLayer(EditorContext& ctx, Origo::RenderContext& rCtx)
	    : m_Context(ctx)
	    , m_RenderContext(rCtx) { };

	void OnUpdate(double dt) override;

private:
	EditorContext& m_Context;
	Origo::RenderContext& m_RenderContext;
};

}
