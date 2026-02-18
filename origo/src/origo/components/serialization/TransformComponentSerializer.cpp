#include "TransformComponentSerializer.h"
#include "origo/components/Transform.h"

namespace Origo {

void TransformComponentSerializer::Serialize(Component* comp, ISerializer& backend) const {
	auto transform = static_cast<TransformComponent*>(comp);

	const Vec3& position = transform->GetPosition();
	const Vec3& rotation = transform->GetRotation();
	const Vec3& scale = transform->GetScale();

	backend.Write("pos_x", position.x);
	backend.Write("pos_y", position.y);
	backend.Write("pos_z", position.z);

	backend.Write("rot_x", rotation.x);
	backend.Write("rot_y", rotation.y);
	backend.Write("rot_z", rotation.z);

	backend.Write("scale_x", scale.x);
	backend.Write("scale_y", scale.y);
	backend.Write("scale_z", scale.z);
}

void TransformComponentSerializer::Deserialize(Component* comp, ISerializer& backend) {
	auto transform = static_cast<TransformComponent*>(comp);

	Vec3 position = transform->GetPosition();
	Vec3 rotation = transform->GetRotation();
	Vec3 scale = transform->GetScale();

	backend.TryRead("pos_x", position.x);
	backend.TryRead("pos_y", position.y);
	backend.TryRead("pos_z", position.z);

	backend.TryRead("rot_x", rotation.x);
	backend.TryRead("rot_y", rotation.y);
	backend.TryRead("rot_z", rotation.z);

	backend.TryRead("scale_x", scale.x);
	backend.TryRead("scale_y", scale.y);
	backend.TryRead("scale_z", scale.z);

	transform->SetPosition(position);
	transform->SetRotation(rotation);
	transform->SetScale(scale);
}

}
