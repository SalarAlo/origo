#include "layer/EditorCameraLayer.h"
#include "origo/input/Input.h"

using namespace Origo;

namespace OrigoEditor {

void EditorCameraLayer::OnAttach() {
	Input::SetCursorMode(Input::CursorMode::Free);
	m_Camera->SetRotationEnabled(true);
	m_Camera->SetMovementEnabled(true);
}

void EditorCameraLayer::OnUpdate(double dt) {
	glm::vec3 direction(0.0f);

	const float speed = Origo::Input::IsKeyPressed(Origo::KeyboardKey::KEY_LEFT_SHIFT)
	    ? FAST_SPEED
	    : NORMAL_SPEED;

	if (Origo::Input::IsKeyPressed(Origo::KeyboardKey::KEY_W))
		direction += m_Camera->GetForward();
	if (Origo::Input::IsKeyPressed(Origo::KeyboardKey::KEY_S))
		direction -= m_Camera->GetForward();
	if (Origo::Input::IsKeyPressed(Origo::KeyboardKey::KEY_D))
		direction += m_Camera->GetRight();
	if (Origo::Input::IsKeyPressed(Origo::KeyboardKey::KEY_A))
		direction -= m_Camera->GetRight();

	if (glm::length(direction) > 0.0f)
		m_Camera->Move(direction * speed);
}

void EditorCameraLayer::OnEvent(Origo::Event& e) {
	Origo::EventDispatcher dispatcher { e };
}
}
