#include "controllers/EditorCameraController.h"
#include "origo/events/Event.h"
#include "origo/input/Input.h"

namespace OrigoEditor {

void EditorCameraController::Update(double dt) {
	glm::vec3 direction(0.0f);

	const float speed = Origo::Input::IsKeyPressed(Origo::KeyboardKey::KEY_LEFT_SHIFT)
	    ? FAST_SPEED
	    : NORMAL_SPEED;

	m_Camera->SetSpeed(speed);

	if (Origo::Input::IsKeyPressed(Origo::KeyboardKey::KEY_W))
		direction += m_Camera->GetForward();
	if (Origo::Input::IsKeyPressed(Origo::KeyboardKey::KEY_S))
		direction -= m_Camera->GetForward();
	if (Origo::Input::IsKeyPressed(Origo::KeyboardKey::KEY_D))
		direction += m_Camera->GetRight();
	if (Origo::Input::IsKeyPressed(Origo::KeyboardKey::KEY_A))
		direction -= m_Camera->GetRight();

	if (glm::length(direction) > 0.0f)
		m_Camera->Move(glm::normalize(direction) * static_cast<float>(dt));
}

void EditorCameraController::HandleEvent(Origo::Event& e) {
	Origo::EventDispatcher dispatcher { e };

	dispatcher.Dispatch<Origo::KeyPressEvent>([&](Origo::KeyPressEvent& e) {
		if (e.GetKeyPressed() == Origo::KeyboardKey::KEY_ESCAPE) {
			Origo::Input::SetCursorMode(Origo::Input::CursorMode::Free);
			m_Camera->SetSensitivity(0);
		} else if (e.GetKeyPressed() == Origo::KeyboardKey::KEY_BACKSPACE) {
			Origo::Input::SetCursorMode(Origo::Input::CursorMode::Locked);
			m_Camera->SetSensitivity(0.1);
		}
	});
}
}
