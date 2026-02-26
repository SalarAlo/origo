#include "layers/EditorCameraLayer.h"

#include "origo/assets/serialization/SceneSerializer.h"

#include "origo/components/Transform.h"

#include "origo/input/Input.h"

using namespace Origo;

namespace OrigoEditor {

void EditorCameraLayer::on_attach() {
	Input::set_cursor_mode(Input::CursorMode::Free);
}

void EditorCameraLayer::on_update(double dt) {
	glm::vec3 move_input(0.0f);

	if (Input::is_key_pressed(KeyboardKey::KEY_W))
		move_input.z += 1.0f;
	if (Input::is_key_pressed(KeyboardKey::KEY_S))
		move_input.z -= 1.0f;
	if (Input::is_key_pressed(KeyboardKey::KEY_D))
		move_input.x += 1.0f;
	if (Input::is_key_pressed(KeyboardKey::KEY_A))
		move_input.x -= 1.0f;
	if (Input::is_key_pressed(KeyboardKey::KEY_E))
		move_input.y += 1.0f;
	if (Input::is_key_pressed(KeyboardKey::KEY_Q))
		move_input.y -= 1.0f;

	float speed = Input::is_key_pressed(KeyboardKey::KEY_LEFT_SHIFT)
	    ? fast_speed
	    : normal_speed;

	if (glm::length(move_input) > 0.0f) {
		m_camera.on_move(glm::normalize(move_input), speed);
	}
}

void EditorCameraLayer::on_event(Event& e) {
	EventDispatcher dispatcher { e };
	dispatcher.dispatch<KeyPressEvent>([this](KeyPressEvent& pressEvent) {
		bool pressed_f { pressEvent.get_key_pressed() == KeyboardKey::KEY_F && pressEvent.get_key_press_type() == KeyPressType::KeyPressStart };
		if (pressed_f) {
			if (!m_context.get_selected_entity())
				return;

			constexpr float base_distance = 2.0f;

			auto& camera = m_context.EditorViewportCamera;

			auto selected_entity = m_context.get_selected_entity().value();
			auto transform { m_context.ActiveScene->get_native_component<TransformComponent>(selected_entity) };

			auto target_pos { transform->get_position() };
			glm::vec3 forward = camera.get_transform().get_forward();

			camera.get_transform().set_position(target_pos - forward * base_distance);
			camera.get_transform().look_at(target_pos);

			camera.update_camera();
		}
	});
}
}
