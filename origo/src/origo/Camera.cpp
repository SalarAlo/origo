#include <glm/gtc/matrix_transform.hpp>

#include "origo/Camera.h"

#include "origo/components/Transform.h"

namespace Origo {

Camera::Camera(float aspect, float nearPlane, float farPlane)
    : m_Aspect(aspect)
    , m_Near(nearPlane)
    , m_Far(farPlane) {
	RecalculateProjection();
}

void Camera::UpdateFromTransform(TransformComponent& transform) {
	m_ViewMatrix = glm::inverse(transform.GetModelMatrix());
}

void Camera::RecalculateProjection() {
	m_ProjectionMatrix = glm::perspective(
	    glm::radians(FOV),
	    m_Aspect,
	    m_Near,
	    m_Far);
}

void Camera::SetNear(float near) {
	m_Near = near;
	RecalculateProjection();
}

void Camera::SetFar(float far) {
	m_Far = far;
	RecalculateProjection();
}

void Camera::SetAspect(float ar) {
	m_Aspect = ar;
	RecalculateProjection();
}

}
