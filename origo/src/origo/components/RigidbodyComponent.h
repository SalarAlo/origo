#pragma once

#include "Jolt/Jolt.h"

#include "Jolt/Physics/Body/BodyManager.h"
#include "Jolt/Physics/Body/MotionQuality.h"

#include "origo/components/Component.h"
#include "origo/core/Typedefs.h"

namespace Origo {

enum class RigidbodyMotionType {
	Static,
	Dynamic,
	Kinematic,
};

struct RigidbodyComponent : public Component {
	RigidbodyMotionType MotionType { RigidbodyMotionType::Dynamic };
	float Mass { 1.0f };
	float Drag { 0.0f };
	float AngularDrag { 0.05f };
	float Friction { 0.2f };
	float Restitution { 0.0f };
	bool UseGravity { true };
	bool AllowSleeping { true };
	bool IsSensor { false };
	JPH::EMotionQuality MotionQuality { JPH::EMotionQuality::Discrete };

	bool HasBody { false };
	JPH::BodyID RuntimeBodyID;
	RigidbodyMotionType RuntimeMotionType { RigidbodyMotionType::Dynamic };
	float RuntimeMass { 1.0f };
	float RuntimeDrag { 0.0f };
	float RuntimeAngularDrag { 0.05f };
	float RuntimeFriction { 0.2f };
	float RuntimeRestitution { 0.0f };
	bool RuntimeUseGravity { true };
	bool RuntimeAllowSleeping { true };
	bool RuntimeIsSensor { false };
	JPH::EMotionQuality RuntimeMotionQuality { JPH::EMotionQuality::Discrete };
	Vec3 RuntimePosition { 0.0f };
	Vec3 RuntimeRotation { 0.0f };
	Vec3 RuntimeScale { 1.0f };

	std::string get_component_name() const { return "Rigidbody"; }
};

}
