#pragma once

#include "origo/components/Transform.h"
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

namespace Origo {

class Camera {
public:
	Camera(float aspect, float nearPlane = 0.1f, float farPlane = 400.0f);

	void SetFOV(float fov);
	float GetFOV() const { return m_FOV; }

	void SetAspect(float aspect);
	float GetAspect() const { return m_Aspect; }

	void SetClipping(float nearPlane, float farPlane);
	Vec2 GetClipping() const { return { m_Near, m_Far }; };

	void UpdateFromTransform(TransformComponent& transform);

	const glm::mat4& GetView() const { return m_ViewMatrix; }
	const glm::mat4& GetProjection() const { return m_ProjectionMatrix; }

private:
	void RecalculateView(const TransformComponent&);
	void RecalculateProjection();

	glm::mat4 m_ViewMatrix;
	glm::mat4 m_ProjectionMatrix;

	float m_FOV { 90.0f };
	float m_Aspect;
	float m_Near;
	float m_Far;
};

}
