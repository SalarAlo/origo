#include "CameraShakeComponentSerializer.h"

#include "origo/components/CameraShakeComponent.h"

namespace Origo {
void CameraShakeComponentSerializer::serialize(Component* component, ISerializer& backend) const {
	auto camera_shake_component { static_cast<CameraShakeComponent*>(component) };

	backend.write("delay", camera_shake_component->ShakeDelay);
	backend.write("amplitude", camera_shake_component->ShakeAmplitude);
	backend.write("frequency", camera_shake_component->ShakeFrequency);
	backend.write("duration", camera_shake_component->ShakeDuration);
};

void CameraShakeComponentSerializer::deserialize(Component* component, ISerializer& backend) {
	auto camera_shake_component { static_cast<CameraShakeComponent*>(component) };

	backend.try_read("delay", camera_shake_component->ShakeDelay);
	backend.try_read("amplitude", camera_shake_component->ShakeAmplitude);
	backend.try_read("frequency", camera_shake_component->ShakeFrequency);
	backend.try_read("duration", camera_shake_component->ShakeDuration);
};

}
