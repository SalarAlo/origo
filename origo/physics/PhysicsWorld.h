#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyID.h>
#include <Jolt/Physics/EActivation.h>
#include <Jolt/Physics/PhysicsSystem.h>

#include "origo/core/RID.h"
#include "origo/core/Typedefs.h"

namespace Origo {

enum class PhysicsLayer {
	NonMoving,
	Moving,
};

class PhysicsWorld {
public:
	PhysicsWorld();
	PhysicsWorld(const PhysicsWorld&) = delete;
	PhysicsWorld& operator=(const PhysicsWorld&) = delete;

	~PhysicsWorld();

	JPH::BodyID create_body(const RID& entity, const JPH::BodyCreationSettings& settings, JPH::EActivation activation = JPH::EActivation::Activate);
	bool destroy_body(const RID& entity);
	void destroy_all_bodies();

	bool has_body(const RID& entity) const;
	JPH::BodyID get_body_id(const RID& entity) const;
	std::vector<RID> get_registered_entities() const;
	JPH::ObjectLayer get_object_layer(PhysicsLayer layer) const;

	JPH::PhysicsSystem& get_system();
	const JPH::PhysicsSystem& get_system() const;

	JPH::BodyInterface& get_body_interface();
	const JPH::BodyInterface& get_body_interface() const;

	void step(float dt);

private:
	struct Impl;
	Scope<Impl> m_impl;
};

}
