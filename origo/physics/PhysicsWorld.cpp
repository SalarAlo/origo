// noincludeformat

#include "origo/physics/PhysicsWorld.h"

#include <Jolt/Jolt.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/RegisterTypes.h>

#include <algorithm>
#include <atomic>
#include <cstdarg>
#include <thread>

namespace Origo {
namespace {
	const JPH::Vec3 cDefaultGravity { 0.0f, -9.81f, 0.0f };

	static void TraceImpl(const char* inFMT, ...) {
		va_list list;
		va_start(list, inFMT);
		char buffer[1024];
		vsnprintf(buffer, sizeof(buffer), inFMT, list);
		va_end(list);

		spdlog::debug("Jolt: {}", buffer);
	}

#ifdef JPH_ENABLE_ASSERTS
	static bool AssertFailedImpl(const char* inExpression, const char* inMessage, const char* inFile, JPH::uint inLine) {
		spdlog::error(
		    "Jolt assertion failed at {}:{}: ({}) {}",
		    inFile,
		    inLine,
		    inExpression,
		    inMessage != nullptr ? inMessage : "");
		return true;
	}
#endif

	struct JoltRuntime {
		JoltRuntime() {
			const int previous_ref_count = s_instance_count.fetch_add(1);
			if (previous_ref_count > 0)
				return;

			JPH::RegisterDefaultAllocator();
			JPH::Trace = TraceImpl;
			JPH_IF_ENABLE_ASSERTS(JPH::AssertFailed = AssertFailedImpl;)

			JPH::Factory::sInstance = new JPH::Factory();
			JPH::RegisterTypes();
		}

		~JoltRuntime() {
			const int remaining_instances = s_instance_count.fetch_sub(1) - 1;
			if (remaining_instances > 0)
				return;

			JPH::UnregisterTypes();
			delete JPH::Factory::sInstance;
			JPH::Factory::sInstance = nullptr;
		}

		static std::atomic<int> s_instance_count;
	};

	std::atomic<int> JoltRuntime::s_instance_count { 0 };

	namespace Layers {
		static constexpr JPH::ObjectLayer NON_MOVING = 0;
		static constexpr JPH::ObjectLayer MOVING = 1;
		static constexpr JPH::ObjectLayer NUM_LAYERS = 2;
	}

	namespace BroadPhaseLayers {
		static constexpr JPH::BroadPhaseLayer NON_MOVING(0);
		static constexpr JPH::BroadPhaseLayer MOVING(1);
		static constexpr JPH::uint NUM_LAYERS = 2;
	}

	class ObjectLayerPairFilterImpl final : public JPH::ObjectLayerPairFilter {
	public:
		bool ShouldCollide(JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2) const override {
			switch (inObject1) {
			case Layers::NON_MOVING:
				return inObject2 == Layers::MOVING;
			case Layers::MOVING:
				return true;
			default:
				JPH_ASSERT(false);
				return false;
			}
		}
	};

	class BPLayerInterfaceImpl final : public JPH::BroadPhaseLayerInterface {
	public:
		BPLayerInterfaceImpl() {
			m_object_to_broad_phase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
			m_object_to_broad_phase[Layers::MOVING] = BroadPhaseLayers::MOVING;
		}

		JPH::uint GetNumBroadPhaseLayers() const override {
			return BroadPhaseLayers::NUM_LAYERS;
		}

		JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override {
			JPH_ASSERT(inLayer < Layers::NUM_LAYERS);
			return m_object_to_broad_phase[inLayer];
		}

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
		const char* GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override {
			switch ((JPH::BroadPhaseLayer::Type)inLayer) {
			case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING:
				return "NON_MOVING";
			case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::MOVING:
				return "MOVING";
			default:
				JPH_ASSERT(false);
				return "INVALID";
			}
		}
#endif

	private:
		JPH::BroadPhaseLayer m_object_to_broad_phase[Layers::NUM_LAYERS];
	};

	class ObjectVsBroadPhaseLayerFilterImpl final : public JPH::ObjectVsBroadPhaseLayerFilter {
	public:
		bool ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const override {
			switch (inLayer1) {
			case Layers::NON_MOVING:
				return inLayer2 == BroadPhaseLayers::MOVING;
			case Layers::MOVING:
				return true;
			default:
				JPH_ASSERT(false);
				return false;
			}
		}
	};

	constexpr JPH::uint cMaxBodies = 65536;
	constexpr JPH::uint cNumBodyMutexes = 0;
	constexpr JPH::uint cMaxBodyPairs = 65536;
	constexpr JPH::uint cMaxContactConstraints = 10240;
	constexpr int cCollisionSteps = 1;
	constexpr int cTempAllocatorSize = 10 * 1024 * 1024;

}

struct PhysicsWorld::Impl {
	Impl()
	    : Runtime {}
	    , TempAllocator(cTempAllocatorSize)
	    , JobSystem(
	          JPH::cMaxPhysicsJobs,
	          JPH::cMaxPhysicsBarriers,
	          std::max(1u, std::thread::hardware_concurrency() > 1 ? std::thread::hardware_concurrency() - 1 : 1u)) {
		System.Init(
		    cMaxBodies,
		    cNumBodyMutexes,
		    cMaxBodyPairs,
		    cMaxContactConstraints,
		    BroadPhaseLayerInterface,
		    ObjectVsBroadPhaseLayerFilter,
		    ObjectLayerPairFilter);
		System.SetGravity(cDefaultGravity);
	}

	JoltRuntime Runtime;
	BPLayerInterfaceImpl BroadPhaseLayerInterface;
	ObjectVsBroadPhaseLayerFilterImpl ObjectVsBroadPhaseLayerFilter;
	ObjectLayerPairFilterImpl ObjectLayerPairFilter;
	JPH::TempAllocatorImpl TempAllocator;
	JPH::JobSystemThreadPool JobSystem;
	JPH::PhysicsSystem System;
	std::unordered_map<RID, JPH::BodyID> BodyIDs {};
};

PhysicsWorld::PhysicsWorld()
    : m_impl(make_scope<Impl>()) { }

PhysicsWorld::~PhysicsWorld() {
	destroy_all_bodies();
}

JPH::BodyID PhysicsWorld::create_body(
    const RID& entity,
    const JPH::BodyCreationSettings& settings,
    JPH::EActivation activation) {
	destroy_body(entity);

	JPH::BodyInterface& body_interface = m_impl->System.GetBodyInterface();
	JPH::BodyID body_id = body_interface.CreateAndAddBody(settings, activation);
	if (body_id.IsInvalid())
		return body_id;

	m_impl->BodyIDs[entity] = body_id;
	return body_id;
}

bool PhysicsWorld::destroy_body(const RID& entity) {
	auto it = m_impl->BodyIDs.find(entity);
	if (it == m_impl->BodyIDs.end())
		return false;

	JPH::BodyInterface& body_interface = m_impl->System.GetBodyInterface();
	if (body_interface.IsAdded(it->second))
		body_interface.RemoveBody(it->second);
	body_interface.DestroyBody(it->second);

	m_impl->BodyIDs.erase(it);
	return true;
}

void PhysicsWorld::destroy_all_bodies() {
	JPH::BodyInterface& body_interface = m_impl->System.GetBodyInterface();

	for (const auto& [_, body_id] : m_impl->BodyIDs) {
		if (body_interface.IsAdded(body_id))
			body_interface.RemoveBody(body_id);
		body_interface.DestroyBody(body_id);
	}

	m_impl->BodyIDs.clear();
}

bool PhysicsWorld::has_body(const RID& entity) const {
	return m_impl->BodyIDs.find(entity) != m_impl->BodyIDs.end();
}

JPH::BodyID PhysicsWorld::get_body_id(const RID& entity) const {
	auto it = m_impl->BodyIDs.find(entity);
	return it != m_impl->BodyIDs.end() ? it->second : JPH::BodyID {};
}

std::vector<RID> PhysicsWorld::get_registered_entities() const {
	std::vector<RID> entities;
	entities.reserve(m_impl->BodyIDs.size());

	for (const auto& [entity, _] : m_impl->BodyIDs)
		entities.push_back(entity);

	return entities;
}

JPH::ObjectLayer PhysicsWorld::get_object_layer(PhysicsLayer layer) const {
	switch (layer) {
	case PhysicsLayer::NonMoving:
		return Layers::NON_MOVING;
	case PhysicsLayer::Moving:
		return Layers::MOVING;
	default:
		JPH_ASSERT(false);
		return Layers::NON_MOVING;
	}
}

JPH::PhysicsSystem& PhysicsWorld::get_system() {
	return m_impl->System;
}

const JPH::PhysicsSystem& PhysicsWorld::get_system() const {
	return m_impl->System;
}

JPH::BodyInterface& PhysicsWorld::get_body_interface() {
	return m_impl->System.GetBodyInterface();
}

const JPH::BodyInterface& PhysicsWorld::get_body_interface() const {
	return m_impl->System.GetBodyInterface();
}

void PhysicsWorld::step(float dt) {
	m_impl->System.Update(dt, cCollisionSteps, &m_impl->TempAllocator, &m_impl->JobSystem);
}

}
