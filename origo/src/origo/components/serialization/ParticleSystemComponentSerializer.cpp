#include "origo/components/serialization/ParticleSystemComponentSerializer.h"

#include "origo/components/ParticleSystemComponent.h"

namespace Origo {

void ParticleSystemComponentSerializer::Serialize(Component* comp, ISerializer& s) const {
	auto ps = static_cast<ParticleSystemComponent*>(comp);
	s.Write("is_looping", ps->IsLooping ? 1 : 0);
	s.Write("spawn_speed", ps->SpawnSpeed);

	auto& startSize = ps->StartSize;
	s.Write("start_size_x", startSize.x);
	s.Write("start_size_y", startSize.y);
	s.Write("start_size_z", startSize.z);
}

void ParticleSystemComponentSerializer::Deserialize(Component* comp, ISerializer& s) {
	auto ps = static_cast<ParticleSystemComponent*>(comp);

	int isLooping { 1 };
	s.TryRead("is_looping", isLooping);
	ps->IsLooping = isLooping;

	s.TryRead("spawn_speed", ps->SpawnSpeed);

	s.Write("start_size_x", ps->StartSize.x);
	s.Write("start_size_y", ps->StartSize.y);
	s.Write("start_size_z", ps->StartSize.z);
}

}
