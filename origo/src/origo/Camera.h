#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include "origo/components/Transform.h"

namespace Origo {

class Camera {
public:
	Camera(float aspect, float nearPlane = 0.1f, float farPlane = 400.0f);

	void UpdateFromTransform(TransformComponent& transform);

	const glm::mat4& GetView() const { return m_ViewMatrix; }
	const glm::mat4& GetProjection() const { return m_ProjectionMatrix; }

	void SetAspect(float ar);
	void SetNear(float near);
	void SetFar(float far);

	float GetNear() const { return m_Near; }
	float GetFar() const { return m_Far; }

	float FOV { 90.0f };

private:
	void RecalculateView(const TransformComponent&);
	void RecalculateProjection();

	glm::mat4 m_ViewMatrix;
	glm::mat4 m_ProjectionMatrix;

	float m_Aspect;
	float m_Near;
	float m_Far;
};

}
