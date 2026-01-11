#include "origo/scene/CameraComponent.h"
#include "origo/scene/NativeComponentRegistry.h"

ORIGO_REGISTER_NATIVE_COMPONENT(Origo::CameraComponent)

namespace Origo {

void CameraComponent::Serialize(ISerializer& backend) const {
	backend.Write("near", m_Camera.GetClipping().x);
	backend.Write("far", m_Camera.GetClipping().y);
	backend.Write("fov", m_Camera.GetFOV());
	backend.Write("aspect", m_Camera.GetAspect());
	backend.Write("is_primary", IsPrimary ? 1 : 0);
}

void CameraComponent::Deserialize(ISerializer& backend) {
	float nearPlane = m_Camera.GetClipping().x;
	float farPlane = m_Camera.GetClipping().y;
	float fov = m_Camera.GetFOV();
	float aspect = m_Camera.GetAspect();
	int isPrimaryInt {};

	backend.TryRead("near", nearPlane);
	backend.TryRead("far", farPlane);
	backend.TryRead("fov", fov);
	backend.TryRead("aspect", aspect);
	backend.TryRead("is_primary", isPrimaryInt);
	IsPrimary = isPrimaryInt != 0;

	m_Camera.SetClipping(nearPlane, farPlane);
	m_Camera.SetFOV(fov);
	m_Camera.SetAspect(aspect);
}
}
