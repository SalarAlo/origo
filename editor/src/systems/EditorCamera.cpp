#include "systems/EditorCamera.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <algorithm>

namespace OrigoEditor {

EditorCamera::EditorCamera(float aspect)
    : m_Camera(aspect) {
	UpdateCamera();
}

void EditorCamera::OnMouseDelta(glm::vec2 delta) {
	m_Yaw -= delta.x * m_MouseSensitivity;
	m_Pitch -= delta.y * m_MouseSensitivity;

	m_Pitch = std::clamp(m_Pitch, -89.0f, 89.0f);

	UpdateCamera();
}

void EditorCamera::OnMove(glm::vec3 direction, float speed) {
	glm::vec3 forward = m_Transform.GetForward();
	glm::vec3 right = m_Transform.GetRight();
	glm::vec3 up = m_Transform.GetUp();

	glm::vec3 delta = direction.x * right + direction.y * up + direction.z * forward;

	m_Transform.Translate(delta * speed);

	UpdateCamera();
}

void EditorCamera::UpdateCamera() {
	glm::quat yaw = glm::angleAxis(glm::radians(m_Yaw), glm::vec3(0.0f, 1.0f, 0.0f));

	glm::vec3 right = yaw * glm::vec3(1.0f, 0.0f, 0.0f);
	glm::quat pitch = glm::angleAxis(glm::radians(m_Pitch), right);

	glm::quat orientation = glm::normalize(pitch * yaw);

	m_Transform.SetRotation(glm::degrees(glm::eulerAngles(orientation)));
	m_Camera.UpdateFromTransform(m_Transform);
}

}
