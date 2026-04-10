#include "layers/EditorCameraLayer.h"

#include "imgui.h"

#include "origo/components/TransformComponent.h"

#include "origo/events/KeyEvent.h"

#include "origo/input/Input.h"

#include "ui/EditorNotificationSystem.h"

using namespace Origo;

namespace OrigoEditor {

namespace {
	bool is_text_input_blocking_camera(const EditorContext& context) {
		return context.is_text_input_active() || ImGui::GetIO().WantTextInput;
	}
}

void EditorCameraLayer::on_attach() {
	Input::set_cursor_mode(Input::CursorMode::Free);
}

void EditorCameraLayer::on_update(double dt) {
	if (is_text_input_blocking_camera(m_context))
		return;

	if (Input::is_key_pressed(KeyboardKey::KEY_LEFT_CONTROL) || Input::is_key_pressed(KeyboardKey::KEY_RIGHT_CONTROL))
		return;

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
		if (pressEvent.get_key_press_type() != KeyPressType::KeyPressStart)
			return;

		if (is_text_input_blocking_camera(m_context))
			return;

		const bool ctrl_pressed = Input::is_key_pressed(KeyboardKey::KEY_LEFT_CONTROL)
		    || Input::is_key_pressed(KeyboardKey::KEY_RIGHT_CONTROL);

		if (ctrl_pressed && pressEvent.get_key_pressed() == KeyboardKey::KEY_D) {
			auto selected_entity = m_context.get_selected_entity();
			if (!selected_entity)
				return;

			auto duplicate = m_context.ActiveScene->duplicate_entity(*selected_entity);
			if (!duplicate)
				return;

			m_context.set_selected_entity(*duplicate);
			EditorNotificationSystem::get_instance().success(
			    "Entity Duplicated",
			    "Created a copy of the selected entity.");
			return;
		}

		bool pressed_f { pressEvent.get_key_pressed() == KeyboardKey::KEY_F };
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
