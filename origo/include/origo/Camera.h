#pragma once

#include "origo/scene/Component.h"
#include "origo/scene/Entity.hpp"
#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>

namespace Origo {

class Camera : public Component {
public:
	Camera(const Ref<Entity>& e, float aspect,
	    const glm::vec3& position = { 0.0f, 0.0f, 0.0f },
	    float yaw = 90.0f,
	    float pitch = 0.0f,
	    float fov = 90.0f,
	    float nearPlane = 0.1f,
	    float farPlane = 400.0f);

	void SetPosition(const glm::vec3& position);

	void Move(const glm::vec3& delta);
	void MoveRight(float step);
	void MoveUp(float step);
	void MoveForward(float step);

	void Rotate(float yawOffset, float pitchOffset);
	void SetYawPitch(float yaw, float pitch);

	void SetFOV(float fov);

	const glm::mat4& GetView() const { return m_ViewMatrix; }
	const glm::mat4& GetProjection() const { return m_ProjectionMatrix; }
	glm::vec3 GetPosition() const { return m_Position; }
	glm::vec3 GetForward() const { return m_Forward; }
	glm::vec3 GetRight() const { return m_Right; }
	glm::vec3 GetUp() const { return m_Up; }

	void SetSpeed(float speed) { m_Speed = speed; }
	void SetSensitivity(float sensitivity) { m_Sensitivity = sensitivity; }
	void SetAspectResolutino(float ar) {
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

	float m_Speed { 0.1f };
	float m_Sensitivity { 0.1f };

	glm::mat4 m_ViewMatrix;
	glm::mat4 m_ProjectionMatrix;
};
}
