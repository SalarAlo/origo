#pragma once

#include "origo/components/Component.h"

namespace Origo {

class TransformComponent : public Component {
public:
	TransformComponent();

	void translate(const Vec3& delta);
	void set_position(const Vec3& position);

	void scale(const Vec3& factor);
	void set_scale(const Vec3& scale);

	void rotate(const Vec3& eulerDegrees);
	void set_rotation(const Vec3& eulerDegrees);
	void look_at(const Vec3& target, const Vec3& up = Vec3(0.0f, 1.0f, 0.0f));

	Vec3 get_forward() const;
	Vec3 get_right() const;
	Vec3 get_up() const;

	void set_from_matrix(const glm::mat4&);

	const Vec3& get_position() const;
	const Vec3& get_rotation() const;
	const Vec3& get_scale() const;

	const glm::mat4& get_model_matrix();

	std::string get_component_name() const override { return "Transform"; }

private:
	void recalculate_model();

	Vec3 m_position;
	Vec3 m_rotation;
	Vec3 m_scale;
	glm::mat4 m_model_matrix;
	bool m_dirty;
};

}
