#include "origo/scene/Transform.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>

namespace Origo {

Transform::Transform(Entity* entity)
    : Component(entity)
    , m_Position(0.0f)
    , m_Rotation(0.0f)
    , m_Scale(1.0f)
    , m_Dirty(true) { }

void Transform::Translate(const glm::vec3& delta) {
	m_Position += delta;
	m_Dirty = true;
}

void Transform::SetPosition(const glm::vec3& position) {
	m_Position = position;
	m_Dirty = true;
}

void Transform::Scale(const glm::vec3& factor) {
	m_Scale *= factor;
	m_Dirty = true;
}

void Transform::SetScale(const glm::vec3& scale) {
	m_Scale = scale;
	m_Dirty = true;
}

void Transform::Rotate(const glm::vec3& eulerDegrees) {
	m_Rotation += eulerDegrees;
	m_Dirty = true;
}

void Transform::SetRotation(const glm::vec3& eulerDegrees) {
	m_Rotation = eulerDegrees;
	m_Dirty = true;
}

const glm::vec3& Transform::GetPosition() const { return m_Position; }
const glm::vec3& Transform::GetRotation() const { return m_Rotation; }
const glm::vec3& Transform::GetScale() const { return m_Scale; }

const glm::mat4& Transform::GetModelMatrix() {
	if (m_Dirty)
		RecalculateModel();
	return m_ModelMatrix;
}

void Transform::RecalculateModel() {
	glm::mat4 translation = glm::translate(glm::mat4(1.0f), m_Position);

	glm::mat4 rotationX = glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation.x), glm::vec3(1, 0, 0));
	glm::mat4 rotationY = glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation.y), glm::vec3(0, 1, 0));
	glm::mat4 rotationZ = glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation.z), glm::vec3(0, 0, 1));

	glm::mat4 rotation = rotationZ * rotationY * rotationX;
	glm::mat4 scale = glm::scale(glm::mat4(1.0f), m_Scale);

	m_ModelMatrix = translation * rotation * scale;
	m_Dirty = false;
}

}
