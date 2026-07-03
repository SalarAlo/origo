#pragma once

namespace Origo {

class Scene;

struct ScriptTimeState {
	float DeltaTime {};
	float FixedDeltaTime {};
};

struct ScriptEngineContext {
	Scene* ActiveScene {};
	ScriptTimeState Time {};
};

class NativeBindingRegistry {
public:
	void register_all();
	void set_context(ScriptEngineContext context) { m_context = context; }
	const ScriptEngineContext& context() const { return m_context; }

private:
	void register_transform_api();
	void register_input_api();
	void register_scene_api();
	void register_time_api();
	void register_log_api();

	ScriptEngineContext m_context {};
	bool m_registered {};
};

}
