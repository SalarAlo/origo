#pragma once

#include "origo/core/Layer.h"
#include "state/EditorContext.h"

namespace OrigoEditor {

class SceneLayer : public Origo::Layer {
public:
	SceneLayer(EditorContext& ctx);

	void on_attach() override;
	void on_update(double dt) override;

private:
	EditorContext& m_context;

	uint32_t m_vertex_layout_id = 0;
	uint32_t m_heap_id = 0;
	size_t m_vertex_stride = 0;
};

}
