#pragma once

#include "origo/Camera.h"
#include "origo/core/Layer.h"

namespace OrigoEditor {
class EditorCameraLayer : public Origo::Layer {
public:
	EditorCameraLayer(Origo::Camera* camera)
	    : m_Camera(camera) { };

	void OnAttach() override;
	void OnUpdate(double dt) override;
	void OnEvent(Origo::Event& e) override;

private:
	Origo::Camera* m_Camera;

	static constexpr float NORMAL_SPEED = 10.0f;
	static constexpr float FAST_SPEED = NORMAL_SPEED * 5.0f;
};
}
