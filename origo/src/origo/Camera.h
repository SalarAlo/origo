#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include "origo/components/Transform.h"

namespace Origo {

class Camera {
public:
	Camera(float aspect, float nearPlane = 0.1f, float farPlane = 400.0f);

	void update_from_transform(TransformComponent& transform);

	const glm::mat4& get_view() const { return m_view_matrix; }
	const glm::mat4& get_projection() const { return m_projection_matrix; }

	void set_aspect(float ar);
	void set_near(float near);
	void set_far(float far);

	float get_near() const { return m_near; }
	float get_far() const { return m_far; }

	float FOV { 90.0f };

private:
	void recalculate_view(const TransformComponent&);
	void recalculate_projection();

	glm::mat4 m_view_matrix;
	glm::mat4 m_projection_matrix;

	float m_aspect;
	float m_near;
	float m_far;
};

}
