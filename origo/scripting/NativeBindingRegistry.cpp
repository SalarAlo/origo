#include "origo/scripting/NativeBindingRegistry.h"

namespace Origo {

void NativeBindingRegistry::register_all() {
	if (m_registered)
		return;

	register_transform_api();
	register_input_api();
	register_scene_api();
	register_time_api();
	register_log_api();
	m_registered = true;
}

void NativeBindingRegistry::register_transform_api() {
	// Hook Noct Entity/Transform native bindings here when the upgraded runtime exposes native module registration.
}

void NativeBindingRegistry::register_input_api() {
	// Hook Input.is_key_down(Key) here.
}

void NativeBindingRegistry::register_scene_api() {
	// Hook Scene.spawn and Scene.destroy here.
}

void NativeBindingRegistry::register_time_api() {
	// Hook Time.delta_time and Time.fixed_delta_time here.
}

void NativeBindingRegistry::register_log_api() {
	// Hook Log.info/warn/error here.
}

}
