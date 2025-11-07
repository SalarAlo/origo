#include "origo/Camera.h"
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
    , m_Far(farPlane)
    , m_IsPositionLocked(false)
    , m_IsRotationLocked(false) {
	UpdateVectors();
	UpdateView();
	UpdateProjection();
}

void Camera::SetPosition(const glm::vec3& position) {
	if (m_IsPositionLocked)
		return;

	m_Position = position;
	UpdateView();
}

void Camera::Move(const glm::vec3& delta) {
	if (m_IsPositionLocked)
		return;

	m_Position += delta;
	UpdateView();
}

void Camera::MoveRight(float step) {
	Move(m_Right * step);
}

void Camera::MoveUp(float step) {
	Move(m_Up * step);
}

void Camera::MoveForward(float step) {
	Move(m_Forward * step);
}

void Camera::Rotate(float yawOffset, float pitchOffset) {
	if (m_IsRotationLocked)
		return;

	m_Yaw += yawOffset;
	m_Pitch += pitchOffset;

	if (m_Pitch > 89.0f)
		m_Pitch = 89.0f;
	if (m_Pitch < -89.0f)
		m_Pitch = -89.0f;

	UpdateVectors();
	UpdateView();
}

void Camera::SetYawPitch(float yaw, float pitch) {
	if (m_IsRotationLocked)
		return;
	m_Yaw = yaw;
	m_Pitch = pitch;
	UpdateVectors();
	UpdateView();
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
	glm::vec3 forward;
	forward.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
	forward.y = sin(glm::radians(m_Pitch));
	forward.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
	m_Forward = glm::normalize(forward);

	m_Right = glm::normalize(glm::cross(m_Forward, { 0.0f, 1.0f, 0.0f }));
	m_Up = glm::normalize(glm::cross(m_Right, m_Forward));
}

}
