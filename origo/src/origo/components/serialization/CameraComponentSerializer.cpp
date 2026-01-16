#include "origo/components/CameraComponent.h"
#include "CameraComponentSerializer.h"

namespace Origo {

void CameraComponentSerializer::Serialize(Component* comp, ISerializer& backend) const {
	auto cameraComponent { static_cast<CameraComponent*>(comp) };
	auto camera { cameraComponent->GetCamera() };

	backend.Write("near", camera.GetClipping().x);
	backend.Write("far", camera.GetClipping().y);
	backend.Write("fov", camera.GetFOV());
	backend.Write("aspect", camera.GetAspect());
	backend.Write("is_primary", cameraComponent->IsPrimary ? 1 : 0);
}

void CameraComponentSerializer::Deserialize(Component* comp, ISerializer& backend) {
	auto cameraComponent { static_cast<CameraComponent*>(comp) };
	auto camera { cameraComponent->GetCamera() };

	float nearPlane = camera.GetClipping().x;
	float farPlane = camera.GetClipping().y;
	float fov = camera.GetFOV();
	float aspect = camera.GetAspect();
	int isPrimaryInt {};

	backend.TryRead("near", nearPlane);
	backend.TryRead("far", farPlane);
	backend.TryRead("fov", fov);
	backend.TryRead("aspect", aspect);
	backend.TryRead("is_primary", isPrimaryInt);
	cameraComponent->IsPrimary = isPrimaryInt != 0;

	camera.SetClipping(nearPlane, farPlane);
	camera.SetFOV(fov);
	camera.SetAspect(aspect);
};

}
