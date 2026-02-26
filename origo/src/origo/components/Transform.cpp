#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Transform.h"

#include "NativeComponentRegistry.h"

#include "glm/gtc/quaternion.hpp"

#include "origo/components/serialization/TransformComponentSerializer.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Origo {

TransformComponent::TransformComponent()
    : m_position(0.0f)
    , m_rotation(0.0f)
    , m_scale(1.0f)
    , m_dirty(true) { }

void TransformComponent::translate(const Vec3& delta) {
	m_position += delta;
	m_dirty = true;
}

void TransformComponent::set_position(const Vec3& position) {
	m_position = position;
	m_dirty = true;
}

void TransformComponent::scale(const Vec3& factor) {
	m_scale *= factor;
	m_dirty = true;
}

void TransformComponent::set_scale(const Vec3& scale) {
	m_scale = scale;
	m_dirty = true;
}

void TransformComponent::rotate(const Vec3& eulerDegrees) {
	m_rotation += eulerDegrees;
	m_dirty = true;
}

void TransformComponent::set_rotation(const Vec3& eulerDegrees) {
	m_rotation = eulerDegrees;
	m_dirty = true;
}

const Vec3& TransformComponent::get_position() const { return m_position; }
const Vec3& TransformComponent::get_rotation() const { return m_rotation; }
const Vec3& TransformComponent::get_scale() const { return m_scale; }

void TransformComponent::set_from_matrix(const glm::mat4& m) {
	m_position = Vec3(m[3]);

	Vec3 col0 = Vec3(m[0]);
	Vec3 col1 = Vec3(m[1]);
	Vec3 col2 = Vec3(m[2]);

	m_scale.x = glm::length(col0);
	m_scale.y = glm::length(col1);
	m_scale.z = glm::length(col2);

	if (m_scale.x != 0.0f)
		col0 /= m_scale.x;
	if (m_scale.y != 0.0f)
		col1 /= m_scale.y;
	if (m_scale.z != 0.0f)
		col2 /= m_scale.z;

	glm::mat3 rot_mat(col0, col1, col2);
	glm::quat q = glm::quat_cast(rot_mat);

	m_rotation = glm::degrees(glm::eulerAngles(q));

	m_model_matrix = m;
	m_dirty = false;
}

const glm::mat4& TransformComponent::get_model_matrix() {
	if (m_dirty)
		recalculate_model();

	return m_model_matrix;
}

void TransformComponent::recalculate_model() {
	glm::mat4 translation = glm::translate(glm::mat4(1.0f), m_position);

	glm::quat q = glm::quat(glm::radians(m_rotation));
	glm::mat4 rotation = glm::toMat4(q);

	glm::mat4 scale = glm::scale(glm::mat4(1.0f), m_scale);

	m_model_matrix = translation * rotation * scale;
	m_dirty = false;
}

void TransformComponent::look_at(const Vec3& target, const Vec3&) {
	Vec3 dir = target - m_position;
	float len = glm::length(dir);
	if (len < 0.0001f)
		return;

	dir /= len;

	float yaw = std::atan2(dir.x, -dir.z);
	float pitch = std::asin(dir.y);

	m_rotation.x = glm::degrees(pitch);
	m_rotation.y = glm::degrees(yaw);
	m_rotation.z = 0.0f;

	m_dirty = true;
}

Vec3 TransformComponent::get_forward() const {
	const glm::mat4& m = const_cast<TransformComponent*>(this)->get_model_matrix();
	return -glm::normalize(Vec3(m[2]));
}

Vec3 TransformComponent::get_right() const {
	const glm::mat4& m = const_cast<TransformComponent*>(this)->get_model_matrix();
	return glm::normalize(Vec3(m[0]));
}

Vec3 TransformComponent::get_up() const {
	const glm::mat4& m = const_cast<TransformComponent*>(this)->get_model_matrix();
	return glm::normalize(Vec3(m[1]));
}

ORIGO_REGISTER_NATIVE_COMPONENT(Origo::TransformComponent, Origo::TransformComponentSerializer)

}
