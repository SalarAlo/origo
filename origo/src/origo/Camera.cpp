#include "origo/Camera.h"
#include "origo/events/MouseEvent.h"
#include "origo/events/WindowEvent.h"
#include "origo/scene/Component.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>

namespace Origo {

Camera::Camera(const Ref<Entity>& e, float aspect, const glm::vec3& position, float yaw, float pitch, float fov, float nearPlane, float farPlane)
    : Component(e)
    , m_Position(position)
    , m_Yaw(yaw)
    , m_Pitch(pitch)
    , m_FOV(fov)
    , m_Aspect(aspect)
    , m_Near(nearPlane)
    , m_Far(farPlane) {
	UpdateVectors();
	UpdateView();
	UpdateProjection();
}

void Camera::SetPosition(const glm::vec3& position) {
	m_Position = position;
	UpdateView();
}

void Camera::Move(const glm::vec3& delta) {
	m_Position += delta * m_Speed;
	UpdateView();
}

void Camera::MoveRight(float step) {
	Move(m_Right * step * m_Speed);
}

void Camera::MoveUp(float step) {
	Move(m_Up * step * m_Speed);
}

void Camera::MoveForward(float step) {
	Move(m_Forward * step * m_Speed);
}

void Camera::Rotate(float yawOffset, float pitchOffset) {
	m_Yaw += yawOffset;
	m_Pitch += pitchOffset;

	// Clamp pitch to avoid gimbal flip
	if (m_Pitch > 89.0f)
		m_Pitch = 89.0f;
	if (m_Pitch < -89.0f)
		m_Pitch = -89.0f;

	UpdateVectors();
	UpdateView();
}

void Camera::SetYawPitch(float yaw, float pitch) {
	m_Yaw = yaw;
	m_Pitch = pitch;
	UpdateVectors();
	UpdateView();
}

void Camera::OnEvent(Event& event) {
	EventDispatcher dispatcher { event };

	dispatcher.Dispatch<WindowResizeEvent>([&](auto& resEvent) {
		auto size = resEvent.GetSize();
		m_Aspect = static_cast<float>(size.x) / static_cast<float>(size.y);
		UpdateProjection();

		// Reset mouse tracking after resize
		m_FirstMouseEvent = true;
	});

	dispatcher.Dispatch<MouseMoveEvent>([&](MouseMoveEvent& moveEvent) {
		const auto& coords = moveEvent.GetCoordinate();

		if (m_FirstMouseEvent) {
			m_LastMousePos = coords;
			m_FirstMouseEvent = false;
			return;
		}

		float dx = m_LastMousePos.x - coords.x;
		float dy = m_LastMousePos.y - coords.y;

		Rotate(-dx * m_Sensitivity, dy * m_Sensitivity);

		m_LastMousePos = coords;
	});
}

void Camera::SetFOV(float fov) {
	m_FOV = fov;
	UpdateProjection();
}

void Camera::UpdateView() {
	m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Forward, m_Up);
}

void Camera::UpdateProjection() {
	m_ProjectionMatrix = glm::perspective(glm::radians(m_FOV), m_Aspect, m_Near, m_Far);
}

void Camera::UpdateVectors() {
	// Calculate the new forward vector from yaw/pitch
	glm::vec3 forward;
	forward.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
	forward.y = sin(glm::radians(m_Pitch));
	forward.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
	m_Forward = glm::normalize(forward);

	// Right and Up vectors
	m_Right = glm::normalize(glm::cross(m_Forward, { 0.0f, 1.0f, 0.0f }));
	m_Up = glm::normalize(glm::cross(m_Right, m_Forward));
}

} // namespace Origo
