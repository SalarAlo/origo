#include "origo/scene/Transform.h"
#include "glm/gtc/quaternion.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Origo {

Transform::Transform()
    : m_Position(0.0f)
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

void Transform::SetFromMatrix(const glm::mat4& m) {
	m_Position = glm::vec3(m[3]);

	glm::vec3 col0 = glm::vec3(m[0]);
	glm::vec3 col1 = glm::vec3(m[1]);
	glm::vec3 col2 = glm::vec3(m[2]);

	m_Scale.x = glm::length(col0);
	m_Scale.y = glm::length(col1);
	m_Scale.z = glm::length(col2);

	if (m_Scale.x != 0.0f)
		col0 /= m_Scale.x;
	if (m_Scale.y != 0.0f)
		col1 /= m_Scale.y;
	if (m_Scale.z != 0.0f)
		col2 /= m_Scale.z;

	glm::mat3 rotMat(col0, col1, col2);
	glm::quat q = glm::quat_cast(rotMat);

	m_Rotation = glm::degrees(glm::eulerAngles(q));

	m_ModelMatrix = m;
	m_Dirty = false;
}

const glm::mat4& Transform::GetModelMatrix() {
	if (m_Dirty)
		RecalculateModel();

	return m_ModelMatrix;
}

void Transform::RecalculateModel() {
	glm::mat4 translation = glm::translate(glm::mat4(1.0f), m_Position);

	glm::quat q = glm::quat(glm::radians(m_Rotation));
	glm::mat4 rotation = glm::toMat4(q);

	glm::mat4 scale = glm::scale(glm::mat4(1.0f), m_Scale);

	m_ModelMatrix = translation * rotation * scale;
	m_Dirty = false;
}

void Transform::LookAt(const glm::vec3& target, const glm::vec3&) {
	glm::vec3 dir = target - m_Position;
	float len = glm::length(dir);
	if (len < 0.0001f)
		return;

	dir /= len;

	float yaw = std::atan2(dir.x, -dir.z);
	float pitch = std::asin(dir.y);

	m_Rotation.x = glm::degrees(pitch);
	m_Rotation.y = glm::degrees(yaw);
	m_Rotation.z = 0.0f;

	m_Dirty = true;
}

glm::vec3 Transform::GetForward() const {
	const glm::mat4& m = const_cast<Transform*>(this)->GetModelMatrix();
	return -glm::normalize(glm::vec3(m[2]));
}

glm::vec3 Transform::GetRight() const {
	const glm::mat4& m = const_cast<Transform*>(this)->GetModelMatrix();
	return glm::normalize(glm::vec3(m[0]));
}

glm::vec3 Transform::GetUp() const {
	const glm::mat4& m = const_cast<Transform*>(this)->GetModelMatrix();
	return glm::normalize(glm::vec3(m[1]));
}
}
