#include "origo/components/serialization/ParticleSystemComponentSerializer.h"

#include "origo/components/particle_system/ParticleSystemComponent.h"
#include "origo/components/particle_system/SanitizeParticleEmissionShape.h"
#include "origo/components/particle_system/SerializeEmissionShape.h"

namespace Origo {

void ParticleSystemComponentSerializer::serialize(Component* comp, ISerializer& s) const {
	auto* ps = static_cast<ParticleSystemComponent*>(comp);

	s.write("is_looping", ps->IsLooping ? 1 : 0);
	s.write("start_delay", ps->StartDelay);
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

	s.begin_array("bursts");
	for (const auto& burst : ps->Bursts) {
		s.begin_array_object();
		s.write("time", burst.Time);
		s.write("count", burst.Count);
		s.end_array_object();
	}
	s.end_array();
}

void ParticleSystemComponentSerializer::deserialize(Component* comp, ISerializer& s) {
	auto* ps = static_cast<ParticleSystemComponent*>(comp);

	int is_looping { 1 };
	s.try_read("is_looping", is_looping);
	ps->IsLooping = is_looping;

	s.try_read("start_delay", ps->StartDelay);
	s.try_read("spawn_rate", ps->SpawnRate);

	float unused_loop_duration = 0.0f;
	s.try_read("loop_duration", unused_loop_duration);

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
	std::visit(SanitizeParticleEmissionShape {}, ps->Shape);
	s.end_object();

	ps->Bursts.clear();
	s.begin_array("bursts");
	while (s.try_begin_array_object_read()) {
		ParticleBurst burst {};
		s.try_read("time", burst.Time);
		s.try_read("count", burst.Count);
		ps->Bursts.push_back(burst);
		s.end_array_object();
	}
	s.end_array();
}

}
