#include "origo/Camera.h"
#include "origo/scene/Transform.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Origo {

Camera::Camera(float aspect, float nearPlane, float farPlane)
    : m_Aspect(aspect)
    , m_Near(nearPlane)
    , m_Far(farPlane) {
	RecalculateProjection();
}

void Camera::UpdateFromTransform(Transform& transform) {
	m_ViewMatrix = glm::inverse(transform.GetModelMatrix());
}

void Camera::SetFOV(float fov) {
	m_FOV = fov;
	RecalculateProjection();
}

void Camera::SetAspect(float aspect) {
	m_Aspect = aspect;
	RecalculateProjection();
}

void Camera::SetClipping(float nearPlane, float farPlane) {
	m_Near = nearPlane;
	m_Far = farPlane;
	RecalculateProjection();
}

void Camera::RecalculateProjection() {
	m_ProjectionMatrix = glm::perspective(
	    glm::radians(m_FOV),
	    m_Aspect,
	    m_Near,
	    m_Far);
}

}
