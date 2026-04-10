#include <glm/gtc/matrix_transform.hpp>

#include "origo/Camera.h"

#include "origo/components/TransformComponent.h"

namespace Origo {

Camera::Camera(float aspect, float nearPlane, float farPlane)
    : m_aspect(aspect)
    , m_near(nearPlane)
    , m_far(farPlane) {
	recalculate_projection();
}

void Camera::update_from_transform(TransformComponent& transform) {
	m_view_matrix = glm::inverse(transform.get_model_matrix());
}

void Camera::recalculate_projection() {
	m_projection_matrix = glm::perspective(
	    glm::radians(FOV),
	    m_aspect,
	    m_near,
	    m_far);
}

void Camera::set_near(float near) {
	m_near = near;
	recalculate_projection();
}

void Camera::set_far(float far) {
	m_far = far;
	recalculate_projection();
}

void Camera::set_aspect(float ar) {
	m_aspect = ar;
	recalculate_projection();
}

}
