#pragma once

#include "state/EditorContext.h"
#include "origo/core/Layer.h"

namespace OrigoEditor {

class UpdateLayer : public Origo::Layer {
public:
	UpdateLayer(EditorContext& ctx)
	    : m_Context(ctx) { };

	void OnUpdate(double dt) override;

private:
	EditorContext& m_Context;
};

}
