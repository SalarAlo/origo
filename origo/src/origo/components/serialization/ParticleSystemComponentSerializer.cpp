#include "origo/components/serialization/ParticleSystemComponentSerializer.h"

#include "origo/components/particle_system/ParticleSystemComponent.h"

namespace Origo {

void ParticleSystemComponentSerializer::Serialize(Component* comp, ISerializer& s) const {
	auto* ps = static_cast<ParticleSystemComponent*>(comp);

	s.Write("is_looping", ps->IsLooping ? 1 : 0);
	s.Write("spawn_rate", ps->SpawnRate);

	s.Write("start_size", ps->StartSize);
	s.Write("end_size", ps->EndSize);

	s.Write("lifetime_min", ps->LifetimeMin);
	s.Write("lifetime_max", ps->LifetimeMax);

	s.Write("initial_speed_min", ps->InitialSpeedMin);
	s.Write("initial_speed_max", ps->InitialSpeedMax);

	s.Write("use_gravity", ps->UseGravity ? 1 : 0);
	s.Write("gravity_force_factor", ps->GravityForceFactor);
	s.Write("drag", ps->Drag);

	if (ps->ParticleMesh.has_value()) {
		auto uuid = AssetManager::GetInstance().GetUUID(ps->ParticleMesh.value());
		if (uuid.has_value())
			s.Write("particle_mesh", uuid->ToString());
	}

	// TODO: Serialize Emission Shape
}

void ParticleSystemComponentSerializer::Deserialize(Component* comp, ISerializer& s) {
	auto* ps = static_cast<ParticleSystemComponent*>(comp);

	int isLooping { 1 };
	s.TryRead("is_looping", isLooping);
	ps->IsLooping = isLooping;

	s.TryRead("spawn_rate", ps->SpawnRate);

	s.TryRead("start_size", ps->StartSize);
	s.TryRead("end_size", ps->EndSize);

	s.TryRead("lifetime_min", ps->LifetimeMin);
	s.TryRead("lifetime_max", ps->LifetimeMax);

	s.TryRead("initial_speed_min", ps->InitialSpeedMin);
	s.TryRead("initial_speed_max", ps->InitialSpeedMax);

	int useGravity { 0 };
	s.TryRead("use_gravity", useGravity);
	ps->UseGravity = useGravity;

	s.TryRead("gravity_force_factor", ps->GravityForceFactor);
	s.TryRead("drag", ps->Drag);

	if (std::string meshID; s.TryRead("particle_mesh", meshID)) {
		auto uuid = UUID::FromString(meshID);
		auto handle = AssetManager::GetInstance().GetHandleByUUID(uuid);
		ps->ParticleMesh = handle;
	}

	// TODO: Deserialize Emission Shape
}

}
