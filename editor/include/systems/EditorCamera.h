#pragma once

#include "origo/Camera.h"
#include "origo/scene/Transform.h"

namespace OrigoEditor {

class EditorCamera {
public:
	EditorCamera(float aspect = 1.0f);

	void OnMouseDelta(glm::vec2 delta);
	void OnMove(glm::vec3 direction, float speed);

	const Origo::Camera& GetCamera() const { return m_Camera; }
	Origo::Camera& GetCamera() { return m_Camera; }

	const Origo::Transform& GetTransform() const { return m_Transform; }
	Origo::Transform& GetTransform() { return m_Transform; }

	void UpdateCamera();

private:
	Origo::Camera m_Camera;
	Origo::Transform m_Transform;

	float m_Yaw { 90.0f };
	float m_Pitch { 0.0f };

	float m_MouseSensitivity { 1.0f };
};

}
