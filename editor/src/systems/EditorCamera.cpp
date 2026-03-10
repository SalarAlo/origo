#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <algorithm>

#include "systems/EditorCamera.h"

namespace OrigoEditor {

EditorCamera::EditorCamera(float aspect)
    : m_camera(aspect) {
	update_camera();
}

void EditorCamera::on_mouse_delta(glm::vec2 delta) {
	m_yaw -= delta.x * m_mouse_sensitivity;
	m_pitch -= delta.y * m_mouse_sensitivity;

	m_pitch = std::clamp(m_pitch, -89.0f, 89.0f);

	update_camera();
}

void EditorCamera::on_move(glm::vec3 direction, float speed) {
	glm::vec3 forward = m_transform.get_forward();
	glm::vec3 right = m_transform.get_right();
	glm::vec3 up = m_transform.get_up();

	glm::vec3 delta = direction.x * right + direction.y * up + direction.z * forward;

	m_transform.translate(delta * speed);

	update_camera();
}

void EditorCamera::update_camera() {
	glm::quat yaw = glm::angleAxis(glm::radians(m_yaw), glm::vec3(0.0f, 1.0f, 0.0f));

	glm::vec3 right = yaw * glm::vec3(1.0f, 0.0f, 0.0f);
	glm::quat pitch = glm::angleAxis(glm::radians(m_pitch), right);

	glm::quat orientation = glm::normalize(pitch * yaw);

	m_transform.set_rotation(glm::degrees(glm::eulerAngles(orientation)));
	m_camera.update_from_transform(m_transform);
}

}
