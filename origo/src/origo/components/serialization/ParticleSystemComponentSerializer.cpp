#include "origo/components/serialization/ParticleSystemComponentSerializer.h"

#include "origo/components/particle_system/ParticleSystemComponent.h"
#include "origo/components/particle_system/SerializeEmissionShape.h"

namespace Origo {

void ParticleSystemComponentSerializer::serialize(Component* comp, ISerializer& s) const {
	auto* ps = static_cast<ParticleSystemComponent*>(comp);

	s.write("is_looping", ps->IsLooping ? 1 : 0);
	s.write("spawn_rate", ps->SpawnRate);

	s.write("start_size", ps->StartSize);
	s.write("end_size", ps->EndSize);

	s.write("lifetime_min", ps->LifetimeMin);
	s.write("lifetime_max", ps->LifetimeMax);

	s.write("initial_speed_min", ps->InitialSpeedMin);
	s.write("initial_speed_max", ps->InitialSpeedMax);

	s.write("use_gravity", ps->UseGravity ? 1 : 0);
	s.write("gravity_force_factor", ps->GravityForceFactor);
	s.write("drag", ps->Drag);

	if (ps->ParticleMesh.has_value()) {
		auto uuid = AssetManager::get_instance().get_uuid(ps->ParticleMesh.value());
		if (uuid.has_value())
			s.write("particle_mesh", uuid->to_string());
	}

	if (ps->ParticleMaterial.has_value()) {
		auto uuid = AssetManager::get_instance().get_uuid(ps->ParticleMaterial.value());
		if (uuid.has_value())
			s.write("particle_material", uuid->to_string());
	}

	s.begin_object("shape");
	auto shape_serializer { SerializeEmissionShape { s } };
	std::visit(shape_serializer, ps->Shape);
	s.end_object();
}

void ParticleSystemComponentSerializer::deserialize(Component* comp, ISerializer& s) {
	auto* ps = static_cast<ParticleSystemComponent*>(comp);

	int is_looping { 1 };
	s.try_read("is_looping", is_looping);
	ps->IsLooping = is_looping;

	s.try_read("spawn_rate", ps->SpawnRate);

	s.try_read("start_size", ps->StartSize);
	s.try_read("end_size", ps->EndSize);

	s.try_read("lifetime_min", ps->LifetimeMin);
	s.try_read("lifetime_max", ps->LifetimeMax);

	s.try_read("initial_speed_min", ps->InitialSpeedMin);
	s.try_read("initial_speed_max", ps->InitialSpeedMax);

	int use_gravity { 0 };
	s.try_read("use_gravity", use_gravity);
	ps->UseGravity = use_gravity;

	s.try_read("gravity_force_factor", ps->GravityForceFactor);
	s.try_read("drag", ps->Drag);

	if (std::string mesh_id; s.try_read("particle_mesh", mesh_id)) {
		auto uuid = UUID::from_string(mesh_id);
		auto handle = AssetManager::get_instance().get_handle_by_uuid(uuid);
		ps->ParticleMesh = handle;
	}

	if (std::string material_id; s.try_read("particle_material", material_id)) {
		auto uuid = UUID::from_string(material_id);
		auto handle = AssetManager::get_instance().get_handle_by_uuid(uuid);
		ps->ParticleMaterial = handle;
	}

	s.begin_object("shape");
	auto shape_deserializer { SerializeEmissionShape { s } };
	ps->Shape = shape_deserializer();
	s.end_object();
}

}
