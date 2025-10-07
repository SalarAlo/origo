#pragma once

#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>

class Camera {
public:
	Camera(float aspect = 1,
	    const glm::vec3& position = { 0.0f, 0.0f, 0.0f },
	    float yaw = 90.0f, // facing -Z
	    float pitch = 0.0f,
	    float fov = 90.0f,
	    float nearPlane = 0.01f,
	    float farPlane = 100.0f);

	// --- Movement/orientation ---
	void SetPosition(const glm::vec3& position);

	void Move(const glm::vec3& delta); // relative move
	void MoveRight(float step);
	void MoveUp(float step);
	void MoveForward(float step);

	void Rotate(float yawOffset, float pitchOffset); // mouse look
	void SetYawPitch(float yaw, float pitch);

	// --- Projection controls ---
	void SetAspect(float aspect);
	void SetFOV(float fov);

	// --- Getters ---
	const glm::mat4& GetView() const { return m_ViewMatrix; }
	const glm::mat4& GetProjection() const { return m_ProjectionMatrix; }
	glm::vec3 GetPosition() const { return m_Position; }
	glm::vec3 GetForward() const { return m_Forward; }
	glm::vec3 GetRight() const { return m_Right; }
	glm::vec3 GetUp() const { return m_Up; }

private:
	void UpdateView();
	void UpdateProjection();
	void UpdateVectors(); // recompute forward/right/up from yaw/pitch

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
};
