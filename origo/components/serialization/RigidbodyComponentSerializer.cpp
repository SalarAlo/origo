#include "origo/components/serialization/RigidbodyComponentSerializer.h"

#include <magic_enum/magic_enum.hpp>

#include "origo/components/RigidbodyComponent.h"

namespace Origo {

void RigidbodyComponentSerializer::serialize(Component* comp, ISerializer& s) const {
	auto rb { dynamic_cast<RigidbodyComponent*>(comp) };

	s.write("motion_type", std::string_view(magic_enum::enum_name(rb->MotionType)));
	s.write("mass", rb->Mass);
	s.write("drag", rb->Drag);
	s.write("angular_drag", rb->AngularDrag);
	s.write("friction", rb->Friction);
	s.write("restitution", rb->Restitution);

	s.write("use_gravity", rb->UseGravity);
	s.write("allow_sleeping", rb->AllowSleeping);
	s.write("is_sensor", rb->IsSensor);
	s.write("motion_quality", std::string_view(magic_enum::enum_name(rb->MotionQuality)));
}

void RigidbodyComponentSerializer::deserialize(Component* comp, ISerializer& s) {
	auto rb { dynamic_cast<RigidbodyComponent*>(comp) };

	if (std::string motion_type; s.try_read("motion_type", motion_type)) {
		if (auto parsed = magic_enum::enum_cast<RigidbodyMotionType>(motion_type))
			rb->MotionType = *parsed;
	}

	s.try_read("mass", rb->Mass);
	s.try_read("drag", rb->Drag);
	s.try_read("angular_drag", rb->AngularDrag);
	s.try_read("friction", rb->Friction);
	s.try_read("restitution", rb->Restitution);

	s.try_read("use_gravity", rb->UseGravity);
	s.try_read("allow_sleeping", rb->AllowSleeping);
	s.try_read("is_sensor", rb->IsSensor);

	if (std::string motion_quality; s.try_read("motion_quality", motion_quality)) {
		if (auto parsed = magic_enum::enum_cast<JPH::EMotionQuality>(motion_quality))
			rb->MotionQuality = *parsed;
	}
}

}
