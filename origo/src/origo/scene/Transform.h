#pragma once

#include "origo/scene/Component.h"

namespace Origo {

class Transform : public Component {
public:
	Transform();

	void Translate(const glm::vec3& delta);
	void SetPosition(const glm::vec3& position);

	void Scale(const glm::vec3& factor);
	void SetScale(const glm::vec3& scale);

	void Rotate(const glm::vec3& eulerDegrees);
	void SetRotation(const glm::vec3& eulerDegrees);
	void LookAt(const glm::vec3& target, const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f));

	glm::vec3 GetForward() const;
	glm::vec3 GetRight() const;
	glm::vec3 GetUp() const;

	void SetFromMatrix(const glm::mat4&);

	const glm::vec3& GetPosition() const;
	const glm::vec3& GetRotation() const;
	const glm::vec3& GetScale() const;

	const glm::mat4& GetModelMatrix();

	std::string GetName() const override { return "Transform"; }

private:
	void RecalculateModel();

	glm::vec3 m_Position;
	glm::vec3 m_Rotation;
	glm::vec3 m_Scale;
	glm::mat4 m_ModelMatrix;
	bool m_Dirty;
};

}
