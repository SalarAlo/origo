#pragma once

#include "origo/scene/Entity.hpp"
#include "origo/scene/Component.h"

namespace Origo {

class Transform : public Component {
public:
	Transform(Entity* entity);

	void Translate(const glm::vec3& delta);
	void SetPosition(const glm::vec3& position);

	void Scale(const glm::vec3& factor);
	void SetScale(const glm::vec3& scale);

	void Rotate(const glm::vec3& eulerDegrees);
	void SetRotation(const glm::vec3& eulerDegrees);

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
