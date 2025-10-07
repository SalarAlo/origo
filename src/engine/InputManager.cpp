#include "InputManager.h"
#include "engine/EventSystem.h"

InputManager::InputManager(Camera& camera, EventSystem& eventSystem)
    : m_EventSystem(eventSystem)
    , m_Camera(camera) {
	static float cameraSpeed { 0.1f };
	m_EventSystem.OnResize([&](int w, int h) {
		m_Camera.SetAspect(static_cast<float>(w) / static_cast<float>(h));
	});

	m_EventSystem.OnMouseMove([&](double xpos, double ypos) {
		static double lastX = xpos, lastY = ypos;
		static bool first = true;
		if (first) {
			lastX = xpos;
			lastY = ypos;
			first = false;
		}

		float xoffset = float(xpos - lastX);
		float yoffset = float(lastY - ypos);
		lastX = xpos;
		lastY = ypos;

		float sensitivity = 0.1f;
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		m_Camera.Rotate(xoffset, yoffset);
	});

	m_EventSystem.OnKeyCallback(GLFW_KEY_W, [&]() {
		m_Camera.MoveForward(cameraSpeed);
	});

	m_EventSystem.OnKeyCallback(GLFW_KEY_S, [&]() {
		m_Camera.MoveForward(-cameraSpeed);
	});
	m_EventSystem.OnKeyCallback(GLFW_KEY_A, [&]() {
		m_Camera.MoveRight(-cameraSpeed);
	});

	m_EventSystem.OnKeyCallback(GLFW_KEY_D, [&]() {
		m_Camera.MoveRight(cameraSpeed);
	});

	m_EventSystem.OnKeyCallback(GLFW_KEY_LEFT_SHIFT, [&]() {
		if (cameraSpeed == 0.05f) {
			cameraSpeed *= 15;
		} else {
			cameraSpeed /= 15;
		}
	});
}
