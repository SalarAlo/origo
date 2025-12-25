#include "layer/EditorCameraLayer.h"
#include "origo/input/Input.h"
#include "origo/scene/Transform.h"
#include "state/EditorRuntimeState.h"

using namespace Origo;

namespace OrigoEditor {

void EditorCameraLayer::OnAttach() {
	Input::SetCursorMode(Input::CursorMode::Free);
}

void EditorCameraLayer::OnUpdate(double dt) {
	glm::vec3 moveInput(0.0f);

	if (Input::IsKeyPressed(KeyboardKey::KEY_W))
		moveInput.z += 1.0f;
	if (Input::IsKeyPressed(KeyboardKey::KEY_S))
		moveInput.z -= 1.0f;
	if (Input::IsKeyPressed(KeyboardKey::KEY_D))
		moveInput.x += 1.0f;
	if (Input::IsKeyPressed(KeyboardKey::KEY_A))
		moveInput.x -= 1.0f;
	if (Input::IsKeyPressed(KeyboardKey::KEY_E))
		moveInput.y += 1.0f;
	if (Input::IsKeyPressed(KeyboardKey::KEY_Q))
		moveInput.y -= 1.0f;

	float speed = Input::IsKeyPressed(KeyboardKey::KEY_LEFT_SHIFT)
	    ? FAST_SPEED
	    : NORMAL_SPEED;

	if (glm::length(moveInput) > 0.0f) {
		m_Camera.OnMove(glm::normalize(moveInput), speed);
	}
}

void EditorCameraLayer::OnEvent(Event& e) {
	EventDispatcher dispatcher { e };
	dispatcher.Dispatch<KeyPressEvent>([this](KeyPressEvent& pressEvent) {
		bool pressedF { pressEvent.GetKeyPressed() == KeyboardKey::KEY_F && pressEvent.GetKeyPressType() == KeyPressType::KeyPressStart };
		if (pressedF) {
			if (!m_Context.SelectedEntity)
				return;

			constexpr float BASE_DISTANCE = 2.0f;

			auto& camera = m_Context.EditorViewportCamera;

			auto selectedEntity = m_Context.SelectedEntity.value();
			auto transform { m_Context.EditorScene->GetComponent<Transform>(selectedEntity.GetID()) };

			auto targetPos { transform->GetPosition() };
			glm::vec3 forward = camera.GetTransform().GetForward();

			camera.GetTransform().SetPosition(targetPos - forward * BASE_DISTANCE);
			camera.GetTransform().LookAt(targetPos);

			camera.UpdateCamera();
		}
	});
}
}
