#pragma once

#include "origo/core/Layer.h"
#include "state/EditorContext.h"

namespace OrigoEditor {
class EditorCameraLayer : public Origo::Layer {
public:
	EditorCameraLayer(EditorContext& context)
	    : m_Camera(context.EditorViewportCamera)
	    , m_Context(context) { };

	void OnAttach() override;
	void OnUpdate(double dt) override;
	void OnEvent(Origo::Event& e) override;

private:
	EditorCamera& m_Camera;
	EditorContext& m_Context;

	static constexpr float NORMAL_SPEED = .2f;
	static constexpr float FAST_SPEED = NORMAL_SPEED * 5.0f;
};
}
