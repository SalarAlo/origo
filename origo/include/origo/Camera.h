#pragma once

#include "origo/scene/Transform.h"
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

namespace Origo {

class Camera {
public:
	Camera(float aspect, float nearPlane = 0.1f, float farPlane = 400.0f);

	void SetFOV(float fov);
	float GetFOV() { return m_FOV; }
	void SetAspect(float aspect);
	void SetClipping(float nearPlane, float farPlane);

	void UpdateFromTransform(Transform& transform);

	const glm::mat4& GetView() const { return m_ViewMatrix; }
	const glm::mat4& GetProjection() const { return m_ProjectionMatrix; }

private:
	void RecalculateView(const Transform&);
	void RecalculateProjection();

	glm::mat4 m_ViewMatrix;
	glm::mat4 m_ProjectionMatrix;

	float m_FOV { 90.0f };
	float m_Aspect;
	float m_Near;
	float m_Far;
};

}
