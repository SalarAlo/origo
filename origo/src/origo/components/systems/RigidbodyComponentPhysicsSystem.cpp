#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>
#include <glm/gtc/quaternion.hpp>

#include "origo/components/systems/RigidbodyComponentPhysicsSystem.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "origo/components/BoxColliderComponent.h"
#include "origo/components/RigidbodyComponent.h"
#include "origo/components/TransformComponent.h"
#include "origo/physics/PhysicsWorld.h"
#include "origo/scene/GamePhase.h"
#include "origo/scene/SystemScheduler.h"

namespace Origo {
namespace {
	constexpr float cTransformEpsilon = 0.0001f;

	Vec3 get_collider_scale(const TransformComponent& transform, const BoxColliderComponent& collider) {
		return transform.get_scale() * collider.Size;
	}

	Vec3 get_collider_offset(const TransformComponent& transform, const BoxColliderComponent& collider) {
		return transform.get_scale() * collider.Offset;
	}

	JPH::RVec3 to_jolt_position(const Vec3& position) {
		return JPH::RVec3(position.x, position.y, position.z);
	}

	JPH::Quat to_jolt_rotation(const Vec3& euler_degrees) {
		glm::quat rotation = glm::quat(glm::radians(euler_degrees));
		return JPH::Quat(rotation.x, rotation.y, rotation.z, rotation.w).Normalized();
	}

	Vec3 to_origo_position(JPH::RVec3Arg position) {
		return Vec3(position.GetX(), position.GetY(), position.GetZ());
	}

	Vec3 to_origo_rotation(JPH::QuatArg rotation) {
		glm::quat q(rotation.GetW(), rotation.GetX(), rotation.GetY(), rotation.GetZ());
		return glm::degrees(glm::eulerAngles(q));
	}

	JPH::EMotionType get_motion_type(const RigidbodyComponent& rb) {
		switch (rb.MotionType) {
		case RigidbodyMotionType::Static:
			return JPH::EMotionType::Static;
		case RigidbodyMotionType::Dynamic:
			return JPH::EMotionType::Dynamic;
		case RigidbodyMotionType::Kinematic:
			return JPH::EMotionType::Kinematic;
		default:
			return JPH::EMotionType::Dynamic;
		}
	}

	PhysicsLayer get_physics_layer(const RigidbodyComponent& rb) {
		return get_motion_type(rb) == JPH::EMotionType::Static ? PhysicsLayer::NonMoving : PhysicsLayer::Moving;
	}

	bool requires_body_rebuild(const RigidbodyComponent& rb) {
		return rb.MotionType != rb.RuntimeMotionType
		    || rb.Mass != rb.RuntimeMass
		    || rb.Drag != rb.RuntimeDrag
		    || rb.AngularDrag != rb.RuntimeAngularDrag
		    || rb.Friction != rb.RuntimeFriction
		    || rb.Restitution != rb.RuntimeRestitution
		    || rb.UseGravity != rb.RuntimeUseGravity
		    || rb.AllowSleeping != rb.RuntimeAllowSleeping
		    || rb.IsSensor != rb.RuntimeIsSensor
		    || rb.MotionQuality != rb.RuntimeMotionQuality;
	}

	bool nearly_equal(const Vec3& lhs, const Vec3& rhs) {
		return glm::all(glm::lessThanEqual(glm::abs(lhs - rhs), Vec3(cTransformEpsilon)));
	}

	bool transform_scale_changed(const RigidbodyComponent& rb, const TransformComponent& transform) {
		return !nearly_equal(rb.RuntimeScale, transform.get_scale());
	}

	bool collider_changed(const RigidbodyComponent& rb, const BoxColliderComponent& collider) {
		return !nearly_equal(rb.RuntimeColliderOffset, collider.Offset)
		    || !nearly_equal(rb.RuntimeColliderSize, collider.Size);
	}

	bool transform_pose_changed(const RigidbodyComponent& rb, const TransformComponent& transform) {
		return !nearly_equal(rb.RuntimePosition, transform.get_position())
		    || !nearly_equal(rb.RuntimeRotation, transform.get_rotation());
	}

	void sync_transform_to_body(
	    PhysicsWorld& physics_world,
	    JPH::BodyID body_id,
	    const TransformComponent& transform,
	    const RigidbodyComponent& rb) {
		JPH::BodyInterface& body_interface = physics_world.get_body_interface();
		const JPH::RVec3 position = to_jolt_position(transform.get_position());
		const JPH::Quat rotation = to_jolt_rotation(transform.get_rotation());

		if (get_motion_type(rb) == JPH::EMotionType::Static) {
			body_interface.SetPositionAndRotation(
			    body_id,
			    position,
			    rotation,
			    JPH::EActivation::DontActivate);
			return;
		}

		// A transform edit on a simulated body is an explicit teleport/reset request.
		body_interface.SetPositionRotationAndVelocity(
		    body_id,
		    position,
		    rotation,
		    JPH::Vec3::sZero(),
		    JPH::Vec3::sZero());
	}

	void sync_runtime_settings(
	    RigidbodyComponent& rb,
	    const TransformComponent& transform,
	    const BoxColliderComponent& collider,
	    JPH::BodyID body_id) {
		rb.HasBody = !body_id.IsInvalid();
		rb.RuntimeBodyID = body_id;
		rb.RuntimeMotionType = rb.MotionType;
		rb.RuntimeMass = rb.Mass;
		rb.RuntimeDrag = rb.Drag;
		rb.RuntimeAngularDrag = rb.AngularDrag;
		rb.RuntimeFriction = rb.Friction;
		rb.RuntimeRestitution = rb.Restitution;
		rb.RuntimeUseGravity = rb.UseGravity;
		rb.RuntimeAllowSleeping = rb.AllowSleeping;
		rb.RuntimeIsSensor = rb.IsSensor;
		rb.RuntimeMotionQuality = rb.MotionQuality;
		rb.RuntimePosition = transform.get_position();
		rb.RuntimeRotation = transform.get_rotation();
		rb.RuntimeScale = transform.get_scale();
		rb.RuntimeColliderOffset = collider.Offset;
		rb.RuntimeColliderSize = collider.Size;
	}

	JPH::BodyCreationSettings make_body_settings(
	    const PhysicsWorld& physics_world,
	    const TransformComponent& transform,
	    const BoxColliderComponent& collider,
	    const RigidbodyComponent& rb) {
		Vec3 half_extent = glm::max(glm::abs(get_collider_scale(transform, collider)) * 0.5f, Vec3(0.05f));
		Vec3 offset = get_collider_offset(transform, collider);

		JPH::RefConst<JPH::Shape> shape = new JPH::BoxShape(JPH::Vec3(half_extent.x, half_extent.y, half_extent.z));
		if (!nearly_equal(offset, Vec3(0.0f))) {
			shape = new JPH::RotatedTranslatedShape(
			    JPH::Vec3(offset.x, offset.y, offset.z),
			    JPH::Quat::sIdentity(),
			    shape);
		}

		JPH::BodyCreationSettings settings(
		    shape.GetPtr(),
		    to_jolt_position(transform.get_position()),
		    to_jolt_rotation(transform.get_rotation()),
		    get_motion_type(rb),
		    physics_world.get_object_layer(get_physics_layer(rb)));

		settings.mLinearDamping = rb.Drag;
		settings.mAngularDamping = rb.AngularDrag;
		settings.mGravityFactor = rb.UseGravity ? 1.0f : 0.0f;
		settings.mFriction = rb.Friction;
		settings.mRestitution = rb.Restitution;
		settings.mAllowSleeping = rb.AllowSleeping;
		settings.mIsSensor = rb.IsSensor;
		settings.mMotionQuality = rb.MotionQuality;

		if (get_motion_type(rb) == JPH::EMotionType::Dynamic && rb.Mass > 0.0f) {
			settings.mOverrideMassProperties = JPH::EOverrideMassProperties::CalculateInertia;
			settings.mMassPropertiesOverride.mMass = rb.Mass;
		}

		return settings;
	}

}

void RigidbodyComponentPhysicsSystem::update(Scene* scene, float dt) {
	auto& physics_world = scene->get_physics_world();

	for (const RID& entity : physics_world.get_registered_entities()) {
		if (!scene->has_native_component<RigidbodyComponent>(entity)
		    || !scene->has_native_component<TransformComponent>(entity)
		    || !scene->has_native_component<BoxColliderComponent>(entity)) {
			physics_world.destroy_body(entity);
		}
	}

	scene->view<RigidbodyComponent, TransformComponent, BoxColliderComponent>(
	    [&](RID entity, RigidbodyComponent& rb, TransformComponent& transform, BoxColliderComponent& collider) {
		    if (!physics_world.has_body(entity)
		        || requires_body_rebuild(rb)
		        || transform_scale_changed(rb, transform)
		        || collider_changed(rb, collider)) {
			    JPH::BodyID body_id = physics_world.create_body(
			        entity,
			        make_body_settings(physics_world, transform, collider, rb));

			    sync_runtime_settings(rb, transform, collider, body_id);
			    return;
		    }

		    JPH::BodyID body_id = physics_world.get_body_id(entity);
		    if (transform_pose_changed(rb, transform)) {
			    sync_transform_to_body(physics_world, body_id, transform, rb);
		    }

		    sync_runtime_settings(rb, transform, collider, body_id);
	    });

	physics_world.step(dt);

	scene->view<RigidbodyComponent, TransformComponent, BoxColliderComponent>(
		    [&](RID entity, RigidbodyComponent& rb, TransformComponent& transform, BoxColliderComponent& collider) {
			    JPH::BodyID body_id = physics_world.get_body_id(entity);
			    if (body_id.IsInvalid()) {
				    rb.HasBody = false;
				    rb.RuntimeBodyID = JPH::BodyID {};
				    rb.RuntimeMotionType = rb.MotionType;
				    return;
			    }

		    JPH::RVec3 position;
		    JPH::Quat rotation;
		    physics_world.get_body_interface().GetPositionAndRotation(body_id, position, rotation);

		    transform.set_position(to_origo_position(position));
		    transform.set_rotation(to_origo_rotation(rotation));

		    sync_runtime_settings(rb, transform, collider, body_id);
	    });
}

}

REGISTER_UPDATE_SYSTEM(
    Origo::GamePhase::UpdateGame,
    Origo::RigidbodyComponentPhysicsSystem)
