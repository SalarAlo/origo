#pragma once

#include "origo/Camera.h"
#include "origo/events/Event.h"

namespace OrigoEditor {

class EditorCameraController {

public:
	explicit EditorCameraController(Origo::Ref<Origo::Camera> camera)
	    : m_Camera(std::move(camera)) { }

	void Update(double dt);

	void HandleEvent(Origo::Event& e);

private:
	Origo::Ref<Origo::Camera> m_Camera;
	static constexpr float NORMAL_SPEED = 10.0f;
	static constexpr float FAST_SPEED = NORMAL_SPEED * 5.0f;
};

}
