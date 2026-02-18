#pragma once

#include "origo/components/Component.h"

namespace Origo {

class TransformComponent : public Component {
public:
	TransformComponent();

	void Translate(const Vec3& delta);
	void SetPosition(const Vec3& position);

	void Scale(const Vec3& factor);
	void SetScale(const Vec3& scale);

	void Rotate(const Vec3& eulerDegrees);
	void SetRotation(const Vec3& eulerDegrees);
	void LookAt(const Vec3& target, const Vec3& up = Vec3(0.0f, 1.0f, 0.0f));

	Vec3 GetForward() const;
	Vec3 GetRight() const;
	Vec3 GetUp() const;

	void SetFromMatrix(const glm::mat4&);

	const Vec3& GetPosition() const;
	const Vec3& GetRotation() const;
	const Vec3& GetScale() const;

	const glm::mat4& GetModelMatrix();

	std::string GetComponentName() const override { return "Transform"; }

private:
	void RecalculateModel();

	Vec3 m_Position;
	Vec3 m_Rotation;
	Vec3 m_Scale;
	glm::mat4 m_ModelMatrix;
	bool m_Dirty;
};

}
