#pragma once

#include "origo/core/Layer.h"
#include "state/EditorContext.h"

namespace OrigoEditor {
class EditorCameraLayer : public Origo::Layer {
public:
	EditorCameraLayer(EditorContext& context)
	    : m_camera(context.EditorViewportCamera)
	    , m_context(context) { };

	void on_attach() override;
	void on_update(double dt) override;
	void on_event(Origo::Event& e) override;

private:
	EditorCamera& m_camera;
	EditorContext& m_context;

	static constexpr float normal_speed = .2f;
	static constexpr float fast_speed = normal_speed * 5.0f;
};
}
