#pragma once

#include "origo/Camera.h"

#include "origo/components/TransformComponent.h"

namespace OrigoEditor {

class EditorCamera {
public:
	EditorCamera(float aspect = 1.0f);

	void on_mouse_delta(glm::vec2 delta);
	void on_move(glm::vec3 direction, float speed);

	const Origo::Camera& get_camera() const { return m_camera; }
	Origo::Camera& get_camera() { return m_camera; }

	const Origo::TransformComponent& get_transform() const { return m_transform; }
	Origo::TransformComponent& get_transform() { return m_transform; }

	void update_camera();

private:
	Origo::Camera m_camera;
	Origo::TransformComponent m_transform;

	float m_yaw { 90.0f };
	float m_pitch { 0.0f };

	float m_mouse_sensitivity { 1.0f };
};

}
