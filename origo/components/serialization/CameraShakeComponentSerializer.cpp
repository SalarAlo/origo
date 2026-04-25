#include "origo/components/serialization/CameraShakeComponentSerializer.h"

#include "origo/components/CameraShakeComponent.h"

namespace Origo {

void CameraShakeComponentSerializer::serialize(Component* comp, ISerializer& backend) const {
	auto camera_shake { static_cast<CameraShakeComponent*>(comp) };

	backend.write("amplitude", camera_shake->Amplitude);
	backend.write("frequency", camera_shake->Frequency);
	backend.write("delay", camera_shake->Delay);
	backend.write("duration", camera_shake->Duration);
}

void CameraShakeComponentSerializer::deserialize(Component* comp, ISerializer& backend) {
	auto camera_shake { static_cast<CameraShakeComponent*>(comp) };

	backend.try_read("amplitude", camera_shake->Amplitude);
	backend.try_read("frequency", camera_shake->Frequency);
	backend.try_read("delay", camera_shake->Delay);
	backend.try_read("duration", camera_shake->Duration);
}

}
