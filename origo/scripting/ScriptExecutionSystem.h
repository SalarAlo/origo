#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "origo/core/RID.h"
#include "origo/scripting/ReflectedFieldStorage.h"
#include "origo/scripting/ScriptAttachmentManager.h"

namespace Origo {

class Scene;

using RuntimeObjectHandle = UUID;
using ScriptInstanceID = UUID;

enum class ScriptInstanceState {
	Created,
	Started,
	DestroyPending,
	Destroyed
};

struct ScriptInstance {
	ScriptInstanceID ID { UUID::generate() };
	RID Entity {};
	ScriptTypeID Type { UUID::bad() };
	const ScriptDescriptor* Descriptor {};
	RuntimeObjectHandle Object { UUID::bad() };
	ReflectedFieldStorage Fields {};
	ScriptInstanceState State { ScriptInstanceState::Created };
};

struct ScriptEvent {
	enum class Kind {
		Collision,
		TriggerEnter,
		TriggerExit
	};

	Kind EventKind { Kind::Collision };
	RID Receiver {};
	RID Other {};
};

class ScriptDispatcher {
public:
	void enqueue(ScriptEvent event);
	void clear();
	void dispatch(class ScriptExecutionSystem& scripts);

private:
	std::vector<ScriptEvent> m_events {};
};

class ScriptExecutionSystem {
public:
	void start_scene(Scene& scene, const ScriptAttachmentManager& attachments);
	void stop_scene();

	void on_script_added(RID entity, const ScriptAttachmentData& attachment);
	void on_script_removed(RID entity, ScriptTypeID type);
	void on_entity_removed(RID entity);

	void update(float dt);
	void fixed_update(float dt);
	void dispatch_event(const ScriptEvent& event);

	ScriptDispatcher& dispatcher() { return m_dispatcher; }
	const std::vector<ScriptInstance>& instances() const { return m_instances; }

private:
	ScriptInstance* find_instance(RID entity, ScriptTypeID type);
	void create_instance(RID entity, const ScriptAttachmentData& attachment);
	void destroy_instance(ScriptInstance& instance, bool invoke_destroy);
	void invoke_lifecycle(ScriptInstance& instance, RuntimeFunctionHandle handle, float dt = 0.0f, RID other = {});
	void execute_lifecycle_body(ScriptInstance& instance, std::string_view body, float dt, RID other);
	void warn_once(const std::string& key, const std::string& message);

	std::vector<ScriptInstance> m_instances {};
	std::unordered_map<RID, std::vector<ScriptInstanceID>> m_instances_by_entity {};
	std::unordered_set<std::string> m_reported_warnings {};
	ScriptDispatcher m_dispatcher {};
	Scene* m_scene {};
	bool m_scene_running {};
	float m_last_delta {};
	float m_fixed_delta {};
	float m_elapsed {};
	std::uint64_t m_frame {};
};

}
