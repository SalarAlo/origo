#include "CameraComponentSerializer.h"

#include "origo/components/CameraComponent.h"

namespace Origo {

void CameraComponentSerializer::serialize(Component* comp, ISerializer& backend) const {
	auto camera_component { static_cast<CameraComponent*>(comp) };
	auto& camera { camera_component->CameraObj };

	backend.write("near", camera.get_near());
	backend.write("far", camera.get_far());
	backend.write("fov", camera.FOV);
	backend.write("is_primary", camera_component->IsPrimary ? 1 : 0);
}

void CameraComponentSerializer::deserialize(Component* comp, ISerializer& backend) {
	auto camera_component { static_cast<CameraComponent*>(comp) };
	auto& camera { camera_component->CameraObj };

	int is_primary_int {};
	float near {};
	float far {};

	backend.try_read("near", near);
	backend.try_read("far", far);
	backend.try_read("fov", camera.FOV);
	backend.try_read("is_primary", is_primary_int);

	camera_component->IsPrimary = is_primary_int != 0;
	camera_component->CameraObj.set_near(near);
	camera_component->CameraObj.set_far(far);
};

}
