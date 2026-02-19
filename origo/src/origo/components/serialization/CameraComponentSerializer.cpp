#include "CameraComponentSerializer.h"

#include "origo/components/CameraComponent.h"

namespace Origo {

void CameraComponentSerializer::Serialize(Component* comp, ISerializer& backend) const {
	auto cameraComponent { static_cast<CameraComponent*>(comp) };
	auto& camera { cameraComponent->CameraObj };

	backend.Write("near", camera.GetNear());
	backend.Write("far", camera.GetFar());
	backend.Write("fov", camera.FOV);
	backend.Write("is_primary", cameraComponent->IsPrimary ? 1 : 0);
}

void CameraComponentSerializer::Deserialize(Component* comp, ISerializer& backend) {
	auto cameraComponent { static_cast<CameraComponent*>(comp) };
	auto& camera { cameraComponent->CameraObj };

	int isPrimaryInt {};
	float near {};
	float far {};

	backend.TryRead("near", near);
	backend.TryRead("far", far);
	backend.TryRead("fov", camera.FOV);
	backend.TryRead("is_primary", isPrimaryInt);

	cameraComponent->IsPrimary = isPrimaryInt != 0;
	cameraComponent->CameraObj.SetNear(near);
	cameraComponent->CameraObj.SetFar(far);
};

}
