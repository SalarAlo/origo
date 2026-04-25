#include "TransformComponentSerializer.h"

#include "origo/components/TransformComponent.h"

namespace Origo {

void TransformComponentSerializer::serialize(Component* comp, ISerializer& backend) const {
	auto transform = static_cast<TransformComponent*>(comp);

	const Vec3& position = transform->get_position();
	const Vec3& rotation = transform->get_rotation();
	const Vec3& scale = transform->get_scale();

	backend.write("pos", position);
	backend.write("rot", rotation);
	backend.write("scale", scale);
}

void TransformComponentSerializer::deserialize(Component* comp, ISerializer& backend) {
	auto transform = static_cast<TransformComponent*>(comp);

	Vec3 position = transform->get_position();
	Vec3 rotation = transform->get_rotation();
	Vec3 scale = transform->get_scale();

	backend.try_read("pos", position);
	backend.try_read("rot", rotation);
	backend.try_read("scale", scale);

	transform->set_position(position);
	transform->set_rotation(rotation);
	transform->set_scale(scale);
}
}
