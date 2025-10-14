#pragma once

#include "origo/scene/Component.h"
#include <glm/glm.hpp>

namespace Origo {

class Transform : public Component {
public:
	Transform(class Entity& entity);

	void Translate(const glm::vec3& delta);
	void SetPosition(const glm::vec3& position);

	void Scale(const glm::vec3& factor);
	void SetScale(const glm::vec3& scale);

	void Rotate(const glm::vec3& eulerDegrees);
	void SetRotation(const glm::vec3& eulerDegrees);

	const glm::vec3& GetPosition() const;
	const glm::vec3& GetRotation() const;
	const glm::vec3& GetScale() const;

	const glm::mat4& GetModelMatrix();

private:
	void RecalculateModel();

	glm::vec3 m_Position;
	glm::vec3 m_Rotation;
	glm::vec3 m_Scale;
	glm::mat4 m_ModelMatrix;
	bool m_Dirty;
};

}
