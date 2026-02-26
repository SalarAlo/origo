#include "TransformComponentSerializer.h"

#include "origo/components/Transform.h"

namespace Origo {

void TransformComponentSerializer::serialize(Component* comp, ISerializer& backend) const {
	auto transform = static_cast<TransformComponent*>(comp);

	const Vec3& position = transform->get_position();
	const Vec3& rotation = transform->get_rotation();
	const Vec3& scale = transform->get_scale();

	backend.write("pos_x", position.x);
	backend.write("pos_y", position.y);
	backend.write("pos_z", position.z);

	backend.write("rot_x", rotation.x);
	backend.write("rot_y", rotation.y);
	backend.write("rot_z", rotation.z);

	backend.write("scale_x", scale.x);
	backend.write("scale_y", scale.y);
	backend.write("scale_z", scale.z);
}

void TransformComponentSerializer::deserialize(Component* comp, ISerializer& backend) {
	auto transform = static_cast<TransformComponent*>(comp);

	Vec3 position = transform->get_position();
	Vec3 rotation = transform->get_rotation();
	Vec3 scale = transform->get_scale();

	backend.try_read("pos_x", position.x);
	backend.try_read("pos_y", position.y);
	backend.try_read("pos_z", position.z);

	backend.try_read("rot_x", rotation.x);
	backend.try_read("rot_y", rotation.y);
	backend.try_read("rot_z", rotation.z);

	backend.try_read("scale_x", scale.x);
	backend.try_read("scale_y", scale.y);
	backend.try_read("scale_z", scale.z);

	transform->set_position(position);
	transform->set_rotation(rotation);
	transform->set_scale(scale);
}

}
