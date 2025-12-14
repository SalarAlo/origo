#pragma once

#include "origo/scene/Component.h"
#include "origo/scene/Entity.hpp"
#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>

namespace Origo {

class Camera : public Component {
public:
	Camera(Entity* e,
	    float aspect = 1.0f,
	    const glm::vec3& position = { 0.0f, 0.0f, 0.0f },
	    float yaw = 90.0f,
	    float pitch = 0.0f,
	    float fov = 90.0f,
	    float nearPlane = 0.01f,
	    float farPlane = 400.0f);

	void SetPosition(const glm::vec3& position);

	void Move(const glm::vec3& delta);
	void MoveRight(float step);
	void MoveUp(float step);
	void MoveForward(float step);

	void Rotate(float yawOffset, float pitchOffset);
	void SetYawPitch(float yaw, float pitch);

	void SetRotationEnabled(bool enabled) { m_IsRotationLocked = !enabled; };
	void SetMovementEnabled(bool enabled) { m_IsPositionLocked = !enabled; };

	void SetFOV(float fov);

	const glm::mat4& GetView() const { return m_ViewMatrix; }
	const glm::mat4& GetProjection() const { return m_ProjectionMatrix; }
	glm::vec3 GetPosition() const { return m_Position; }
	glm::vec3 GetForward() const { return m_Forward; }
	glm::vec3 GetRight() const { return m_Right; }
	glm::vec3 GetUp() const { return m_Up; }

	void SetAspectResolution(float ar) {
		m_Aspect = ar;
		UpdateProjection();
	}

	std::string GetName() const override {
		return "Camera";
	}

private:
	void UpdateView();
	void UpdateProjection();
	void UpdateVectors();

private:
	glm::vec3 m_Position;
	glm::vec3 m_Forward;
	glm::vec3 m_Right;
	glm::vec3 m_Up;

	float m_Yaw;
	float m_Pitch;

	float m_FOV;
	float m_Aspect;
	float m_Near;
	float m_Far;

	glm::mat4 m_ViewMatrix;
	glm::mat4 m_ProjectionMatrix;

	bool m_IsRotationLocked;
	bool m_IsPositionLocked;
};
}
